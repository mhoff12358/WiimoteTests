#pragma once

#include "LinearAccelerationReading.h"
#include "Quaternion.h"
#include <cmath>
#include <array>

class LinearAcceleration
{
public:
	LinearAcceleration(const LinearAccelerationReading& raw_reading, const int* acceleration_calibration, const int* gravity_calibration, const float magnitude_cap);
	~LinearAcceleration();

	float magnitude;
	std::array<float, 3> direction;

	void RemoveGravity(const Quaternion& current_orientation);
};

