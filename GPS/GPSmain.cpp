
#include "GPS.h"

#using <System.dll>
#include <Windows.h>
#include <conio.h>
#include <SMStructs.h>
#include <SMObject.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main()
{
    GPS^ gpsdata = gcnew GPS;
    std::cout << "Start GPS" << std::endl;

	gpsdata->setupSharedMemory();
	std::cout << "Successful with shared memory" << std::endl;

    // Declaration
    int Shutdown = 0x00;

    int wait_count = 0;
    int LIMIT = 25;

	int PortNumber = 24000;
	String^ ipAddress = gcnew String("192.168.1.200");

	gpsdata->connect(ipAddress, PortNumber);
	std::cout << "Successful with connection" << std::endl;

    while (1)
    {
		if (!gpsdata->checkHeartbeat())
		{
			//std::cout << "GPS Heartbeat is " << static_cast<unsigned>(gpsdata->checkHeartbeat()) << std::endl;
			gpsdata->setHeartbeat(true);
			wait_count = 0;
		}
		else
		{
			wait_count++;
			if (wait_count > LIMIT)
			{
				std::cout << "Shudown PM" << std::endl;
				gpsdata->getShutdownFlag();
				return 1;
			}
		}
        //std::cout << "Wait Count is " << static_cast<unsigned>(wait_count) << std::endl;

		gpsdata->getData();

        Thread::Sleep(25);
    }

    return 0;
}