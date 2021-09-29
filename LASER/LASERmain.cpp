#include "LASER.h"

#using <System.dll>
#include <Windows.h>
#include <conio.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main()
{
    SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

    //SM seeking access
    PMObj.SMAccess();

    // Declaration
    double TimeStamp;
    __int64 Frequency, Counter;
    int Shutdown = 0x00;

    ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
    QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
    while (1)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
        TimeStamp = (double)Counter / (double)Frequency * 1000; //ms
        Console::WriteLine("Laser time stamp   : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
        Thread::Sleep(25);
        if (PMData->Shutdown.Status) 
        {
            exit(0);
        }
    }

    return 0;
}