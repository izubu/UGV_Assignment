
#include "VehicleControl.h"


int VehicleControl::connect(String^ hostName, int portNumber)
{
	String^ zID = gcnew String("z5267112\n");
	// Pointer to TcpClent type object on managed heap
	std::cout << "create client to store" << std::endl;
	// Create TcpClient object and connect to it
	Client = gcnew TcpClient(hostName, portNumber);
	std::cout << "config connection" << std::endl;
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	std::cout << "create arrays for sending and reading data" << std::endl;
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(zID);

	std::cout << "get stream and use to read/write" << std::endl;
	// Get the network stream object associated with client so we 
	// can use it to read and write
	Stream = Client->GetStream();

	Stream->Write(SendData, 0, SendData->Length);

	std::cout << "server prepares for data" << std::endl;
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);

	std::cout << "read data" << std::endl;
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);

	std::cout << "convert to ASCII" << std::endl;
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	flag = false;

	return 1;
}
int VehicleControl::setupSharedMemory()
{
	ProcessManagementData = new SMObject(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	//SM Creation and seeking access
	ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMData = (ProcessManagement*)ProcessManagementData->pData;

	SensorData = new SMObject(TEXT("VehicleControl"), sizeof(SM_VehicleControl));
	//SM Creation and seeking access
	SensorData->SMCreate();
	SensorData->SMAccess();
	VCData = (SM_VehicleControl*)SensorData->pData;

	flag = false;
	return 1;
}
int VehicleControl::getData()
{
	// YOUR CODE HERE
	return 1;
}
int VehicleControl::checkData()
{
	// YOUR CODE HERE
	return 1;
}
int VehicleControl::sendDataToSharedMemory()
{
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("# Steering: " + VCData->Steering + "Speed: " + VCData->Speed + "Flag: " + flag + " #");
	// String to store received data for display

	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	flag = !flag;

	// Write command asking for data
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
	return 1;
}
bool VehicleControl::getShutdownFlag()
{
	PMData->Shutdown.Status = 0xFF;
	return PMData->Shutdown.Status;
}
int VehicleControl::checkHeartbeat()
{
	return PMData->Heartbeat.Flags.VehicleControl;
}
int VehicleControl::setHeartbeat(bool heartbeat)
{
	PMData->Heartbeat.Flags.VehicleControl = 1;
	return PMData->Heartbeat.Flags.VehicleControl;
}
VehicleControl::~VehicleControl()
{
	Stream->Close();
	Client->Close();
}



unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}