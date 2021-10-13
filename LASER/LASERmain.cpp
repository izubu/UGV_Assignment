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
	
	LASER laserdata;
	std::cout << "Start" << std::endl;

	laserdata.setupSharedMemory();
	std::cout << "Start1" << std::endl;

	// Declaration
	int Shutdown = 0x00;

	int wait_count = 0;
	int LIMIT = 25;
	
	int PortNumber = 23000;
	String^ ipAddress = gcnew String("192.168.1.200");
	
	laserdata.connect(ipAddress, PortNumber);

	std::cout << "Start2" << std::endl;

	//Loop
	while (1)
	{
		if (laserdata.checkHeartbeat())
		{
			std::cout << "LASER Heartbeat is " << static_cast<unsigned>(laserdata.checkHeartbeat()) << std::endl;
			laserdata.setHeartbeat(true);
			wait_count = 0;

		}
		else
		{
			wait_count++;
			if (wait_count > LIMIT)
			{
				std::cout << "Shudown PM" << std::endl;
				laserdata.getShutdownFlag();
				return 1;
			}
		}
		std::cout << "Wait Count is " << static_cast<unsigned>(wait_count) << std::endl;
		std::cout << "Sending to Eternet" << std::endl;

		laserdata.getData();

		laserdata.checkData();

	}

	laserdata.sendDataToSharedMemory();
    
    

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