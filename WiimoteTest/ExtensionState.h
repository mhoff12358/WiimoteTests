#pragma once

#include "Acceleration.h"
#include <iostream>

class ExtensionState
{
public:
	ExtensionState(unsigned char buffer_type, unsigned char* buffer);
	~ExtensionState();

	unsigned char stick_position[2];
	Acceleration acceleration;
	bool c_pressed, z_pressed;
};

