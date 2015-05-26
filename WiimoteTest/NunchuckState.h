#pragma once

#include "Acceleration.h"
#include <iostream>

class NunchuckState
{
public:
	NunchuckState(unsigned char buffer_type, unsigned char* extension_buffer);
	NunchuckState();
	~NunchuckState();

	unsigned char stick_position[2];
	Acceleration acceleration;
	bool c_pressed, z_pressed;
};

