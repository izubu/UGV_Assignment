
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
    SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

    //SM Creation and seeking access
    PMObj.SMCreate();
    PMObj.SMAccess();

    ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

    // Declaration
    int Shutdown = 0x00;

    int wait_count = 0;
    int LIMIT = 25;

    while (1)
    {
        if (PMData->Heartbeat.Flags.VehicleControl == 0)
        {
            std::cout << "Vehicle Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.VehicleControl) << std::endl;
            PMData->Heartbeat.Flags.VehicleControl = 1;
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
    }

    return 0;
}