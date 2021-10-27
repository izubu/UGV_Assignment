
#include "LASER.h"


int LASER::connect(String^ hostName, int portNumber)
{
	String^ zID = gcnew String("z5267112\n");
	// Pointer to TcpClent type object on managed heap
	std::cout << "create arrays for sending and reading data" << std::endl;
	
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");
	std::cout << "Call responseData" << std::endl;
	// String to store received data for display

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
	SensorData->SMAccess();
	LASERData = (SM_Laser*)SensorData->pData;

	return 1;
}
int LASER::getData()
{
	
	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");
	// String to store received data for display
	

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
	double PI = 3.14159265;
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
		/*{
			Range[i] = System::Convert::ToInt32(StringArray[25 + i], 16);
			LASERData->x[i] = Range[i] * sin(double(i) * Resolution * PI / 180.0);
			LASERData->y[i] = -Range[i] * cos(double(i) * Resolution * PI / 180.0);
			std::cout << i + 1 << " (" << LASERData->x[i] << ", " << LASERData->y[i] << ")" << std::endl;
		}*/
		{
			Range[i] = System::Convert::ToInt32(StringArray[25 + i], 16);
			RangeX[i] = Range[i] * sin(double(i) * Resolution * PI / 180.0);
			RangeY[i] = -Range[i] * cos(double(i) * Resolution * PI / 180.0);
			std::cout << i + 1 << " (" << RangeX[i] << ", " << RangeY[i] << ")" << std::endl;
		}
	} 
	else
	{
			std::cout << "Bad data \n";
	}
	return 1;
}
int LASER::sendDataToSharedMemory()
{
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
	return PMData->Heartbeat.Flags.Laser;
}
int LASER::setHeartbeat(bool heartbeat)
{
	PMData->Heartbeat.Flags.Laser = 1;
	return PMData->Heartbeat.Flags.Laser;
}
LASER::~LASER()
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