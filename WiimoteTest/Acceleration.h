#pragma once

#include "WiimoteEnums.h"

class Acceleration
{
public:
	Acceleration(unsigned char* buffer);

	int acceleration[3];
};

