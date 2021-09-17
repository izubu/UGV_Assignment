
#include "LASER.h"


int LASER::connect(String^ hostName, int portNumber)
{
	// YOUR CODE HERE
	return 1;
}
int LASER::setupSharedMemory()
{
	// YOUR CODE HERE
	return 1;
}
int LASER::getData()
{
	// YOUR CODE HERE
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