#pragma once

#include "WiimoteEnums.h"

class MotionPlusState
{
public:
	MotionPlusState(unsigned char buffer_type, unsigned char* extension_buffer);
	MotionPlusState();

	~MotionPlusState();

	// Rotation data is stored as yaw, roll, pitch
	// Rotation mode is true if it is in slow mode
	bool rotation_mode[3];
	int rotation[3];
	bool extension_connected;
};

