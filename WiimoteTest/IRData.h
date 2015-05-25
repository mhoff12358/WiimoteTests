#pragma once

#include "WiimoteEnums.h"

struct IRPoint {
	int point[2];
};

class IRData
{
public:
	IRData(unsigned char* buffer);
	~IRData();

	IRPoint points[4];

private:
	void UnpackData10Bytes(unsigned char* buffer);
	void UnpackData12Bytes(unsigned char* buffer);
	void UnpackData36Bytes(unsigned char* buffer1, unsigned char* buffer2);

	IRPoint UnpackPoint5Bytes(unsigned char* buffer, unsigned char point_num);
};

