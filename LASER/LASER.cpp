
#include "LASER.h"


int LASER::connect(String^ hostName, int portNumber)
{
	String^ zID = gcnew String("z5267112\n");
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;
	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");
	// String to store received data for display
	String^ ResponseData;
	// Create TcpClient object and connect to it
	Client = gcnew TcpClient(hostName, portNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);
	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(zID);

	// Get the network stream object associated with client so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();

	SendData = System::Text::Encoding::ASCII->GetBytes(zID);

	Stream->Write(SendData, 0, SendData->Length);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	return 1;
}
int LASER::setupSharedMemory()
{
	ProcessManagementData = new SMObject(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	//SM Creation and seeking access
	ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMData = (ProcessManagement*)ProcessManagementData->pData;

	SensorData = new SMObject(TEXT("LASER"), sizeof(SM_Laser));
	//SM Creation and seeking access
	SensorData->SMCreate();

	return 1;
}
int LASER::getData()
{
	array<unsigned char>^ SendData;
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");
	// String to store received data for display
	String^ ResponseData;

	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

	// Write command asking for data
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	return 1;
}
int LASER::checkData()
{
	String^ ResponseData;
	if (ResponseData->StartsWith("sRA"))
	{
		array<wchar_t>^ Space = { ' ' };
		array<String^>^ StringArray = ResponseData->Split(Space);

		double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
		double Resolution = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
		int NumRanges = System::Convert::ToInt32(StringArray[25], 16);

		array<double>^ Range = gcnew array<double>(NumRanges);
		array<double>^ RangeX = gcnew array<double>(NumRanges);
		array<double>^ RangeY = gcnew array<double>(NumRanges);

		std::cout << "Converts and prints coordinates" << std::endl;
		for (int i = 0; i < NumRanges; i++)
		{
			Range[i] = System::Convert::ToInt32(StringArray[25 + i], 16);
			RangeX[i] = Range[i] * sin(i * Resolution);
			RangeY[i] = -Range[i] * cos(i * Resolution);
			std::cout << "(" << RangeX[i] << " " << RangeY[i] << ")" << std::endl;
		}
	}
	return 1;
}
int LASER::sendDataToSharedMemory()
{
	Stream->Close();
	Client->Close();

	Console::ReadKey();
	Console::ReadKey();
	return 1;
}
bool LASER::getShutdownFlag()
{
	PMData->Shutdown.Status = 0xFF;
	return PMData->Shutdown.Status;
}
int LASER::checkHeartbeat()
{
	if (PMData->Heartbeat.Flags.Laser == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int LASER::setHeartbeat(bool heartbeat)
{
	PMData->Heartbeat.Flags.Laser = 1;
	return PMData->Heartbeat.Flags.Laser;
}
LASER::~LASER()
{
	// YOUR CODE HERE
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