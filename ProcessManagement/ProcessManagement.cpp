
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

//defining start up sequence
TCHAR Units[10][20] = //
{
	TEXT("GPS.exe"),
	TEXT("Camera.exe"),
	TEXT("Display.exe"),
	TEXT("LASER.exe"),
	TEXT("VehicleControl.exe")
};

int main()
{
	//start all 5 modules
	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement));
	
	array<String^>^ ModuleList = gcnew array<String^>{"LASER", "Display", "VehicleControl", "GPS", "Camera"};
	array<int>^ Critical = gcnew array<int>(ModuleList->Length) { 1, 1, 1, 0, 0 };
	array<Process^>^ ProcessList = gcnew array<Process^>(ModuleList->Length);

	//SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	StartProcesses();

	while (!_kbhit())
	{
		Thread::Sleep(1000);
	}

	PMData->Shutdown.Status = 0xFF;

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
		if (!IsProcessRunning((const char *)Units[i]))
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

