#pragma once

#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class GPS : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int setupSharedMemory() override;
	int getData() override;
	int checkData() override;
	int sendDataToSharedMemory() override;
	bool getShutdownFlag() override;
	int checkHeartbeat() override;
	int setHeartbeat(bool heartbeat) override;
	~GPS();

protected:
	ProcessManagement* PMData;
	SM_GPS* GPSData;
	String^ ResponseData;
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	array<unsigned char>^ Data;
};
