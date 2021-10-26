
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

#define NUM_UNITS 5

bool IsProcessRunning(const char* processName);
void StartProcesses();
void StartProcess(int i);

//defining start up sequence
TCHAR Units[10][20] = //
{
	TEXT("GPS.exe"),
	TEXT("Camera.exe"),
	TEXT("Display.exe"),
	TEXT("VehicleControl.exe"),
	TEXT("LASER.exe")
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
	//std::cout << "STARTED" << std::endl;

	StartProcesses();
	int LIMIT = 20;
	
	array<int>^ wait_count = gcnew array<int>(NUM_UNITS) { 0, 0, 0, 0, 0 };
	//array<int>^ Critical = gcnew array<int>(NUM_UNITS) { 1, 1, 1, 1, 0 };

	while (1)
	{
		if (PMData->Heartbeat.Flags.GPS == 1)
		{
		// std::cout << "GPS Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.GPS) << std::endl;
			PMData->Heartbeat.Flags.GPS = 0; wait_count[0] = 0;
		}
		else
		{
			wait_count[0]++;
			if (wait_count[0] > LIMIT)
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
			if (wait_count[1] > LIMIT)
			{
				std::cout << "Shudown LASER" << std::endl;
				PMData->Shutdown.Status = 0xFF;
				PMData->deadProcess = 3;
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
			if (wait_count[2] > LIMIT)
			{
				std::cout << "Shudown Vehicle Control" << std::endl;
				PMData->Shutdown.Status = 0xFF;
				PMData->deadProcess = 4;
				break;

			}
		}
		if (PMData->Heartbeat.Flags.Display == 1)
		{
			//std::cout << "Laser Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.Display) << std::endl;
			PMData->Heartbeat.Flags.Display = 0; wait_count[3] = 0;
		}
		else
		{
			wait_count[3]++;
			if (wait_count[3] > LIMIT)
			{
				std::cout << "Shudown Display" << std::endl;
				PMData->Shutdown.Status = 0xFF;
				PMData->deadProcess = 2;
				break;

			}
		}
		if (PMData->Heartbeat.Flags.Camera == 1)
		{
			//std::cout << "Laser Heartbeat is " << static_cast<unsigned>(PMData->Heartbeat.Flags.Display) << std::endl;
			PMData->Heartbeat.Flags.Camera = 0; wait_count[4] = 0;
		}
		else
		{
			wait_count[4]++;
			if (wait_count[4] > LIMIT)
			{
				std::cout << "Shudown Camera" << std::endl;
				PMData->Shutdown.Status = 0xFF;
				PMData->deadProcess = 1;
				break;

			}
		}
		if (_kbhit())
		{
			std::cout << "Shudown PM" << std::endl;
			PMData->Shutdown.Status = 0xFF;
			break;
		}
		Thread::Sleep(100);
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
		else
		{
			std::cout << Units[i] << " already running" << std::endl;
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
	else
	{
		std::cout << Units[i] << " already running" << std::endl;
	}
}