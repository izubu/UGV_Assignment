
#include "VehicleControl.h"

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

    VehicleControl^ VCdata = gcnew VehicleControl;
    std::cout << "Start VehicleControl" << std::endl;

    VCdata->setupSharedMemory();
    std::cout << "Successful with shared memory" << std::endl;

    // Declaration
    int Shutdown = 0x00;

    int wait_count = 0;
    int LIMIT = 25;

    int PortNumber = 25000;
    String^ ipAddress = gcnew String("192.168.1.200");

    VCdata->connect(ipAddress, PortNumber);
    std::cout << "Successful with connection" << std::endl;

    while (1)
    {
        if (!VCdata->checkHeartbeat())
        {
            std::cout << "Vehicle Control Heartbeat is " << static_cast<unsigned>(VCdata->checkHeartbeat()) << std::endl;
            VCdata->setHeartbeat(true);
            wait_count = 0;
        }
        else
        {
            wait_count++;
            if (wait_count > LIMIT)
            {
                std::cout << "Shudown PM" << std::endl;
                VCdata->getShutdownFlag();
                return 1;
            }
        }
        std::cout << "Wait Count is " << static_cast<unsigned>(wait_count) << std::endl;

        VCdata->sendDataToSharedMemory();

        Thread::Sleep(25);
    }

    return 0;
}