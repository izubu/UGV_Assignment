
#include "LASER.h"


int LASER::connect(String^ hostName, int portNumber)
{
	//// Pointer to TcpClent type object on managed heap
	//TcpClient^ Client;
	//// arrays of unsigned chars to send and receive data
	//array<unsigned char>^ SendData;
	//array<unsigned char>^ ReadData;
	//// Create TcpClient object and connect to it
	//Client = gcnew TcpClient(hostName, portNumber);
	//// Configure connection
	//Client->NoDelay = true;
	//Client->ReceiveTimeout = 500;//ms
	//Client->SendTimeout = 500;//ms
	//Client->ReceiveBufferSize = 1024;
	//Client->SendBufferSize = 1024;

	return 1;
}
int LASER::setupSharedMemory()
{
	//SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	////SM Creation and seeking access
	//PMObj.SMCreate();
	//PMObj.SMAccess();
	//ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	return 1;
}
int LASER::getData()
{
	//array<unsigned char>^ SendData;
	//// String command to ask for Channel 1 analogue voltage from the PLC
	//// These command are available on Galil RIO47122 command reference manual
	//// available online
	//String^ AskScan = gcnew String("sRN LMDscandata");
	//// String to store received data for display
	//String^ ResponseData;

	//SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);

	//// Write command asking for data
	//Stream->WriteByte(0x02);
	//Stream->Write(SendData, 0, SendData->Length);
	//Stream->WriteByte(0x03);
	//// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	//System::Threading::Thread::Sleep(10);
	//// Read the incoming data
	//Stream->Read(ReadData, 0, ReadData->Length);
	//// Convert incoming data from an array of unsigned char bytes to an ASCII string
	//ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	return 1;
}
int LASER::checkData()
{
	// YOUR CODE HERE
	return 1;
}
int LASER::sendDataToSharedMemory()
{
	// YOUR CODE HERE
	return 1;
}
bool LASER::getShutdownFlag()
{
	// YOUR CODE HERE
	return 1;
}
int LASER::setHeartbeat(bool heartbeat)
{
	// YOUR CODE HERE
	return 1;
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