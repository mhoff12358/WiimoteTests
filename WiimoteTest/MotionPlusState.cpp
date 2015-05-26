#include "stdafx.h"
#include "MotionPlusState.h"


MotionPlusState::MotionPlusState()
{
}

MotionPlusState::MotionPlusState(unsigned char buffer_type, unsigned char* buffer) {
	switch (buffer_type) {
	case MOTION_PLUS: {
		extension_connected = (buffer[4] & 0x01) != 0;
		rotation_mode[0] = (buffer[3] & 0x02) != 0;
		rotation_mode[1] = (buffer[4] & 0x02) != 0;
		rotation_mode[2] = (buffer[3] & 0x01) != 0;
		rotation[0] = buffer[0] + ((buffer[3] & 0xFC) << 8);
		rotation[1] = buffer[1] + ((buffer[4] & 0xFC) << 8);
		rotation[2] = buffer[2] + ((buffer[5] & 0xFC) << 8);
		break;
	}
	}
}

MotionPlusState::~MotionPlusState()
{
}
