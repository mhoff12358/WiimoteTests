#pragma once

#include "WiimoteEnums.h"
#include <iostream>

class Acceleration
{
public:
	Acceleration();
	Acceleration(unsigned char buffer_type, unsigned char* buffer);

	int acceleration[3];
};

