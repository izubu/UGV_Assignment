
#include "GPS.h"
using namespace System::Threading;

#pragma pack(1)
struct DataGPS//112 bytes
{
	unsigned int Header;
	unsigned char Discards1[40];
	double Northing;
	double Easting;
	double Height;
	unsigned char Discards2[40];
	unsigned int Checksum;
};

int GPS::connect(String^ hostName, int portNumber) 
{
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online

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
	std::cout << "create arrays for sending and reading data" << std::endl;
	// Convert string command to an array of unsigned char

	std::cout << "get stream and use to read/write" << std::endl;
	// Get the network stream object associated with client so we 
	// can use it to read and write
	Stream = Client->GetStream();

	Stream->Write(SendData, 0, SendData->Length);

	std::cout << "server prepares for data" << std::endl;
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);


	return 1;
}
int GPS::setupSharedMemory() 
{
	ProcessManagementData = new SMObject(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	//SM Creation and seeking access
	ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMData = (ProcessManagement*)ProcessManagementData->pData;

	SensorData = new SMObject(TEXT("GPS"), sizeof(SM_GPS));
	//SM Creation and seeking access
	SensorData->SMCreate();
	SensorData->SMAccess();
	GPSData = (SM_GPS*)SensorData->pData;

	return 1;
}
int GPS::getData() 
{
	ReadData = gcnew array<unsigned char>(128);
	array<unsigned char>^ Data = gcnew array<unsigned char>(2500);

	DataGPS Data_GPS;

	unsigned char Buffer[sizeof(SM_GPS)];
	unsigned int CalculatedCRC;
	double Northing;
	double Easting;
	double Height;

	/*std::cout << "read data" << std::endl;*/
	// Read the incoming data
	if (Stream->DataAvailable) {
		Stream->Read(ReadData, 0, ReadData->Length);

		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

		std::cout << "Start header trapping" << std::endl;

		unsigned char Data;
		unsigned int Header = 0;
		int element = 0;
		int Start; //Start of data
		do {
			Data = ReadData[element++];
			Header = ((Header << 8) | Data);
		} while (Header != 0xaa44121c);
		Start = element - 4;
		std::cout << "Finish header trapping" << std::endl;

		unsigned char* BytePtr = (unsigned char*)&Data_GPS;
		for (int i = 0; i < sizeof(Data_GPS); i++)
		{
			*(BytePtr++) = ReadData[i];
			Buffer[i] = ReadData[i];
		}

		CalculatedCRC = CalculateBlockCRC32(108, Buffer);

		printf("Calc CRC %X\n", CalculatedCRC);
		/*printf("Checksum is %X\n", Data_GPS.Checksum);*/

		if (CalculatedCRC == Data_GPS.Checksum)
		{
			GPSData->Height = Data_GPS.Height;
			GPSData->Northing = Data_GPS.Northing;
			GPSData->Easting = Data_GPS.Easting;
			printf("Height: %f\nNorthing: %f\nEasting: %f.\n", GPSData->Height, GPSData->Northing, GPSData->Easting);
		}
		else
		{
			Console::WriteLine("Checksum error in GPS data.");
		}
		Thread::Sleep(25);
	}
	return 1;
}
int GPS::checkData() 
{
	// YOUR CODE HERE
	return 1;
}
int GPS::sendDataToSharedMemory() 
{
	/*SM_GPS SM_GPS;
	GPSData->Height = SM_GPS.Height;
	GPSData->Northing = SM_GPS.Northing;
	GPSData->Easting = SM_GPS.Easting;*/
	return 1;
}
bool GPS::getShutdownFlag()
{
	PMData->Shutdown.Status = 0xFF;
	return PMData->Shutdown.Status;
}
int GPS::checkHeartbeat()
{
	return PMData->Heartbeat.Flags.GPS;
}
int GPS::setHeartbeat(bool heartbeat)
{
	PMData->Heartbeat.Flags.GPS = 1;
	return PMData->Heartbeat.Flags.GPS;
}
GPS::~GPS()
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