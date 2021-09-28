
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

    //SM seeking access
    PMObj.SMAccess();

    ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

    // Declaration
    double TimeStamp;
    __int64 Frequency, Counter;
    int Shutdown = 0x00;

    QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
    while (1)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
        TimeStamp = (double)Counter / (double)Frequency * 1000; //ms
        Console::WriteLine("Vehicle Control time stamp   : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
        Thread::Sleep(25);
        if (PMData->Shutdown.Status) {
            exit(-1);
        }
    }

    return 0;
}