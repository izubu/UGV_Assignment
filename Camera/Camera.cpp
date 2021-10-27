#include <zmq.hpp>
#include <Windows.h>

#include "SMStructs.h"
#include "SMFcn.h"
#include "SMObject.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <turbojpeg.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

void display();
void idle();

GLuint tex;

//ZMQ settings
zmq::context_t context(1);
zmq::socket_t subscriber(context, ZMQ_SUB);

int wait_count = 0;

int main(int argc, char** argv)
{
	//Define window size
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	//GL Window setup
	glutInit(&argc, (char**)(argv));
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("MTRN3500 - Camera");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glGenTextures(1, &tex);

	//Socket to talk to server
	subscriber.connect("tcp://192.168.1.200:26000");
	subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	//SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;


	glutMainLoop();
		
	return 1;
}


void display()
{
	//Set camera as gl texture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	//Map Camera to window
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex2f(-1, -1);
	glTexCoord2f(1, 1); glVertex2f(1, -1);
	glTexCoord2f(1, 0); glVertex2f(1, 1);
	glTexCoord2f(0, 0); glVertex2f(-1, 1);
	glEnd();
	glutSwapBuffers();
}
void idle()
{

	//receive from zmq
	zmq::message_t update;
	if (subscriber.recv(&update, ZMQ_NOBLOCK))
	{
		//Receive camera data
		long unsigned int _jpegSize = update.size();
		std::cout << "received " << _jpegSize << " bytes of data\n";
		unsigned char* _compressedImage = static_cast<unsigned char*>(update.data());
		int jpegSubsamp = 0, width = 0, height = 0;

		//JPEG Decompression
		tjhandle _jpegDecompressor = tjInitDecompress();
		tjDecompressHeader2(_jpegDecompressor, _compressedImage, _jpegSize, &width, &height, &jpegSubsamp);
		unsigned char* buffer = new unsigned char[width * height * 3]; //!< will contain the decompressed image
		printf("Dimensions:  %d   %d\n", height, width);
		tjDecompress2(_jpegDecompressor, _compressedImage, _jpegSize, buffer, width, 0/*pitch*/, height, TJPF_RGB, TJFLAG_FASTDCT);
		tjDestroy(_jpegDecompressor);

		//load texture
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer);
		delete[] buffer;
	}

	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	//SM seeking access
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	// Declaration
	int Shutdown = 0x00;

	int LIMIT = 10;

	if (PMData->Heartbeat.Flags.Camera == 0)
	{
			std::cout << "Camera Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.Camera) << std::endl;
			PMData->Heartbeat.Flags.Camera = 1;
			wait_count = 0;
	}
	else
	{
		wait_count++;
		if (wait_count > LIMIT)
		{
			std::cout << "Shudown PM" << std::endl;
			PMData->Shutdown.Status = 0xFF;
			exit(-1);
		}
	}
	std::cout << "Wait Count is " << static_cast<unsigned>(wait_count) << std::endl;
	Thread::Sleep(100);
	display();
}

