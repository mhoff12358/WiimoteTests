#pragma once

#include "LinearAccelerationReading.h"
#include <cmath>
#include <array>

class LinearAcceleration
{
public:
	LinearAcceleration(const LinearAccelerationReading& raw_reading, const int* acceleration_calibration, const int* gravity_calibration);
	~LinearAcceleration();

	std::array<float, 3> acceleration;
	float magnitude;
	std::array<float, 3> direction;
};

