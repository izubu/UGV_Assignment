#pragma once

#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class VehicleControl : public UGV_module
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
	~VehicleControl();

protected:
	ProcessManagement* PMData;
	SM_VehicleControl* VCData;
	String^ ResponseData;
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	bool flag;
};
