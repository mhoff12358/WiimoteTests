#include "stdafx.h"
#include "IRData.h"


IRData::IRData(unsigned char* buffer)
{
	switch (buffer[0]) {
	case DATA_BUT_ACC_IR10_EXT6: {
		UnpackData10Bytes(buffer + 6);
		break;
	}
	case DATA_BUT_IR10_EXT9: {
		UnpackData10Bytes(buffer + 3);
		break;
	}
	case DATA_BUT_ACC_IR12 : {
		UnpackData12Bytes(buffer + 6);
		break;
	}
	case DATA_INTERLEAVE_BOTH: {
		UnpackData36Bytes(buffer + 4, buffer + 22 + 4);
		break;
	}
	}
}

IRData::~IRData()
{
}

void IRData::UnpackData10Bytes(unsigned char* buffer) {
	points[0] = UnpackPoint5Bytes(buffer, 0);
	points[1] = UnpackPoint5Bytes(buffer, 1);
	points[2] = UnpackPoint5Bytes(buffer + 5, 0);
	points[3] = UnpackPoint5Bytes(buffer + 5, 1);
}

void IRData::UnpackData12Bytes(unsigned char* buffer) {

}

void IRData::UnpackData36Bytes(unsigned char* buffer1, unsigned char* buffer2) {

}

IRPoint IRData::UnpackPoint5Bytes(unsigned char* buffer, unsigned char point_num) {
	IRPoint result;
	for (int i = 0; i < 2; i++) { result.point[i] = 0; }
	switch (point_num) {
	case 0:
		result.point[0] = (buffer[0] << 2) + ((buffer[2] & 0x30) >> 4);
		result.point[1] = (buffer[1] << 2) + ((buffer[2] & 0xC0) >> 6);
		break;
	case 1:
		result.point[0] = (buffer[3] << 2) + ((buffer[2] & 0x03));
		result.point[1] = (buffer[4] << 2) + ((buffer[2] & 0x0C) >> 2);
		break;
	}
	if (result.point[0] == 0x3FF) {
		result.point[0] = -1;
		result.point[1] = -1;
	}
	return result;
}