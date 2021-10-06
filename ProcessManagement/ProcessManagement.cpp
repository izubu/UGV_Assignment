
#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include <SMStructs.h>
#include <SMObject.h>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Diagnostics;
using namespace System::Threading;

#define NUM_UNITS 3

bool IsProcessRunning(const char* processName);
void StartProcesses();
void StartProcess(int i);

//defining start up sequence
TCHAR Units[10][20] = //
{
	TEXT("GPS.exe"),
	/*TEXT("Camera.exe"),
	TEXT("Display.exe"),*/
	TEXT("LASER.exe"),
	TEXT("VehicleControl.exe")
};

int main()
{
	//start all 5 modules
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	//SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	PMData->Heartbeat.Status = 0b00000000;

	StartProcesses();
	int wait_count[3] = { 0, 0, 0 };
	int limit_count[3] = { 50, 50, 50 };

	while (1)
	{
		if (PMData->Heartbeat.Flags.GPS == 1)
		{
		std::cout << "GPS Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.GPS) << std::endl;
			PMData->Heartbeat.Flags.GPS = 0; wait_count[0] = 0;
		}
		else
		{
			wait_count[0]++;
			if (wait_count[0] > limit_count[0])
			{
				std::cout << "Shudown GPS" << std::endl;
				StartProcess(0);
				PMData->Heartbeat.Flags.GPS = 0;
				wait_count[0] = 0;
			}
		}
		if (PMData->Heartbeat.Flags.Laser == 1)
		{
			//std::cout << "Laser Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.Laser) << std::endl;
			PMData->Heartbeat.Flags.Laser = 0; wait_count[1] = 0;
		}
		else
		{
			wait_count[1]++;
			if (wait_count[1] > limit_count[1])
			{
				std::cout << "Shudown LASER" << std::endl;
				PMData->Shutdown.Status = 0xFF;
				PMData->deadProcess = 1;
				break;

			}
		}
		if (PMData->Heartbeat.Flags.VehicleControl == 1)
		{ 
		//std::cout << "Laser Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.Laser) << std::endl;
			PMData->Heartbeat.Flags.VehicleControl = 0; wait_count[2] = 0;
		}
		else
		{
			wait_count[2]++;
			if (wait_count[2] > limit_count[2])
			{
				std::cout << "Shudown Vehicle Control" << std::endl;
				PMData->Shutdown.Status = 0xFF;
				PMData->deadProcess = 2;
				break;

			}
		}
		if (_kbhit())
		{
			std::cout << "Shudown PM" << std::endl;
			PMData->Shutdown.Status = 0xFF;
			break;
		}
		Thread::Sleep(25);
	}

	PMData->Shutdown.Status = 0xFF;
	std::cout << Units[PMData->deadProcess] << " has died.\n";
	/*while (1)
	{
		Thread::Sleep(25);
	}*/

	return 0;
}


//Is process running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp((const char *)entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}


void StartProcesses()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	for (int i = 0; i < NUM_UNITS; i++)
	{
		if (!IsProcessRunning((const char*)Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));

			if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
			}
			std::cout << "Started: " << Units[i] << std::endl;
			Sleep(100);
		}
	}
}

void StartProcess(int i)
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	if (!IsProcessRunning((const char*)Units[i]))
	{
		ZeroMemory(&s[i], sizeof(s[i]));
		s[i].cb = sizeof(s[i]);
		ZeroMemory(&p[i], sizeof(p[i]));

		if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
		{
			printf("%s failed (%d).\n", Units[i], GetLastError());
			_getch();
		}
		std::cout << "Started: " << Units[i] << std::endl;
		Sleep(100);
	}
}