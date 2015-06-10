#include "stdafx.h"
#include "LinearAcceleration.h"


LinearAcceleration::LinearAcceleration(const LinearAccelerationReading& raw_reading, const int* acceleration_calibration, const int* gravity_calibration)
{
	magnitude = 0.0f;
	for (int i = 0; i < 3; i++) {
		acceleration[i] = static_cast<float>(raw_reading.acceleration[i] - acceleration_calibration[i]) / (gravity_calibration[i]);
		magnitude += acceleration[i] * acceleration[i];
	}
	magnitude = sqrtf(magnitude);
	for (int i = 0; i < 3; i++) {
		direction[i] = acceleration[i] / magnitude;
	}
}

LinearAcceleration::~LinearAcceleration()
{
}
