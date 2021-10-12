#include "LASER.h"

#using <System.dll>
#include <Windows.h>
#include <conio.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;


int main()
{
	
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	//SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	// Declaration
	int Shutdown = 0x00;

	int wait_count = 0;
	int LIMIT = 25;
	
	// LMS151 port number must be 2111
	int PortNumber = 23000;
	String^ ipAddress = gcnew String("192.168.1.200");
	String^ zID = gcnew String("z5267112\n");
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;

	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");
	// String to store received data for display
	String^ ResponseData;

	// Create TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(zID);

	std::cout << "Process" << std::endl;

	// Get the network streab object associated with clien so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();

	// Authenticate User
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(zID);
	Stream->Write(SendData, 0, SendData->Length);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	// Print the received string on the screen
	/*Console::WriteLine(ResponseData);
	Console::ReadKey();*/

	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);


	//Loop
	while (_kbhit())
	{
		// Write command asking for data
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
		System::Threading::Thread::Sleep(10);
		// Read the incoming data
		Stream->Read(ReadData, 0, ReadData->Length);
		// Convert incoming data from an array of unsigned char bytes to an ASCII string
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		// Print the received string on the screen
		Console::WriteLine(ResponseData);

		if (PMData->Heartbeat.Flags.Laser == 0)
		{
			std::cout << "LASER Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.Laser) << std::endl;
			PMData->Heartbeat.Flags.Laser = 1;
			wait_count = 0;

		}
		else
		{
			wait_count++;
			if (wait_count > LIMIT)
			{
				std::cout << "Shudown PM" << std::endl;
				PMData->Shutdown.Status = 0xFF;
				break;
			}
		}
		std::cout << "Wait Count is " << static_cast<unsigned>(wait_count) << std::endl;
		
		
	}

	Stream->Close();
	Client->Close();

	Console::ReadKey();
	Console::ReadKey();
    
    

    /*while (1)
    {
        if (PMData->Heartbeat.Flags.Laser == 0)
        {
            std::cout << "LASER Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.Laser) << std::endl;
            PMData->Heartbeat.Flags.Laser = 1;
            wait_count = 0;
        }
        else
        {
            wait_count++;
            if (wait_count > LIMIT)
            {
                std::cout << "Shudown PM" << std::endl;
                PMData->Shutdown.Status = 0xFF;
                break;
            }
        }
        std::cout << "Wait Count is " << static_cast<unsigned>(wait_count) << std::endl;
        Thread::Sleep(100);
    }*/
    
    return 0;
}