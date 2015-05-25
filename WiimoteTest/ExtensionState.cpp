#include "stdafx.h"
#include "ExtensionState.h"


ExtensionState::ExtensionState(unsigned char buffer_type, unsigned char* buffer)
{
	unsigned char* extension_buffer;
	switch (buffer_type) {
	case DATA_BUT_EXT8:
	case DATA_BUT_EXT19: {
		extension_buffer = buffer + 3;
		break;
	}
	case DATA_BUT_ACC_EXT16: {
		extension_buffer = buffer + 6;
		break;
	}
	case DATA_BUT_IR10_EXT9: {
		extension_buffer = buffer + 13;
		break;
	}
	case DATA_BUT_ACC_IR10_EXT6: {
		extension_buffer = buffer + 16;
		break;
	}
	default:
		extension_buffer = 0;
		break;
	}
	acceleration = Acceleration(NUNCHUNK, extension_buffer);
	memcpy(stick_position, extension_buffer, 2);
	c_pressed = (extension_buffer[5] & 0x02) == 0;
	z_pressed = (extension_buffer[5] & 0x01) == 0;
}


ExtensionState::~ExtensionState()
{
}
