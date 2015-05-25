#include "stdafx.h"
#include "Acceleration.h"

Acceleration::Acceleration() {
	for (int i = 0; i < 3; i++) {
		acceleration[i] = -1;
	}
}

Acceleration::Acceleration(unsigned char buffer_type, unsigned char* buffer)
{
	switch (buffer_type) {
	case DATA_BUT_ACC:
	case DATA_BUT_ACC_IR12:
	case DATA_BUT_ACC_EXT16:
	case DATA_BUT_ACC_IR10_EXT6: {
		acceleration[0] = ((buffer[3] & 0xFF) << 2) + ((buffer[1] & 0x60) >> 5);
		acceleration[1] = ((buffer[4] & 0xFF) << 2) + ((buffer[2] & 0x40) >> 5);
		acceleration[2] = ((buffer[5] & 0xFF) << 2) + ((buffer[2] & 0x20) >> 4);
		break;
	}
	case DATA_INTERLEAVE_1:
		break;
	case NUNCHUNK: {
		acceleration[0] = ((buffer[2] & 0xFF) << 2) + ((buffer[5] & 0x0C) >> 2);
		acceleration[1] = ((buffer[3] & 0xFF) << 2) + ((buffer[5] & 0x30) >> 4);
		acceleration[2] = ((buffer[4] & 0xFF) << 2) + ((buffer[5] & 0xC0) >> 6);
		break;
	}
	default:
		std::cout << "Attempting to create acceleration with invalid buffer type" << std::endl;
		break;
	}
}

