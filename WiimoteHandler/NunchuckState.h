#pragma once

#include "LinearAccelerationReading.h"
#include <iostream>

class NunchuckState
{
public:
	NunchuckState(unsigned char buffer_type, unsigned char* extension_buffer);
	NunchuckState();
	~NunchuckState();

	unsigned char stick_position[2];
	LinearAccelerationReading acceleration;
	bool c_pressed, z_pressed;
};

