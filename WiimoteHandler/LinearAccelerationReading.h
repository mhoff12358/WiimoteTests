#pragma once

#include "WiimoteEnums.h"
#include <iostream>

class LinearAccelerationReading
{
public:
	LinearAccelerationReading();
	LinearAccelerationReading(unsigned char buffer_type, unsigned char* buffer);

	int acceleration[3];
};

