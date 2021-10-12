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

	std::cout << "LASER not connected" << std::endl;

	// Create TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);

	std::cout << "LASER connected" << std::endl;

	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	std::cout << "LASER configured" << std::endl;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(zID);

	// Get the network stream object associated with client so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();
	
	std::cout << "Unauthenticate User" << std::endl;

	// Authenticate User
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(zID);
	
	std::cout << "Authenticate User" << std::endl;

	Stream->Write(SendData, 0, SendData->Length);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	std::cout << "Changes bytes to ASCII string" << std::endl;

	// Print the received string on the screen
	/*Console::WriteLine(ResponseData);
	Console::ReadKey();*/

	std::cout << "Initalising SendData" << std::endl;

	/*SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);*/


	//Loop
	while (1)
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
				return 1;
			}
		}
		std::cout << "Wait Count is " << static_cast<unsigned>(wait_count) << std::endl;
		std::cout << "Sending to Eternet" << std::endl;

		// Write command asking for data
		Stream->WriteByte(0x02);
		SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
		System::Threading::Thread::Sleep(10);
		
		std::cout << "Reading from Eternet" << std::endl;
		
		// Read the incoming data
		Stream->Read(ReadData, 0, ReadData->Length);

		std::cout << "Converts and prints raw data" << std::endl;

		// Convert incoming data from an array of unsigned char bytes to an ASCII string
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		// Print the received string on the screen
		Console::WriteLine(ResponseData);

		array<wchar_t>^ Space = { ' ' };
		array<String^>^ StringArray = ResponseData->Split(Space);

		double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
		double Resolution = System::Convert::ToInt32(StringArray[24], 16)/10000.0;
		int NumRanges = System::Convert::ToInt32(StringArray[25], 16);

		array<double> ^Range = gcnew array<double>(NumRanges);
		array<double> ^RangeX = gcnew array<double>(NumRanges);
		array<double>^ RangeY = gcnew array<double>(NumRanges);

		std::cout << "Converts and prints coordinates" << std::endl;
		for (int i = 0; i < NumRanges; i++)
		{
			Range[i] = System::Convert::ToInt32(StringArray[25 + i], 16);
			RangeX[i] = Range[i] * sin(i * Resolution);
			RangeY[i] = -Range[i] * cos(i * Resolution);
			std::cout << "(" << RangeX[i] << " " << RangeY[i] << ")" << std::endl;
		}

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