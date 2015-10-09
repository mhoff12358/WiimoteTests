#include "stdafx.h"
#include "LinearAcceleration.h"


LinearAcceleration::LinearAcceleration(const LinearAccelerationReading& raw_reading, const int* acceleration_calibration, const int* gravity_calibration, const float magnitude_cap)
{
	std::array<float, 3> acceleration;
	magnitude = 0.0f;
	for (int i = 0; i < 3; i++) {
		acceleration[i] = static_cast<float>(raw_reading.acceleration[i] - acceleration_calibration[i]) / (gravity_calibration[i]);
		magnitude += acceleration[i] * acceleration[i];
	}
	magnitude = min(sqrtf(magnitude), magnitude_cap);
	for (int i = 0; i < 3; i++) {
		direction[i] = acceleration[i] / magnitude;
	}
}

LinearAcceleration::~LinearAcceleration()
{
}

void LinearAcceleration::RemoveGravity(const Quaternion& current_orientation) {
	std::array<float, 3> relative_gravity = current_orientation.ApplyToVector(std::array<float, 3>({ { 0, 0, 1 } }));
	std::array<float, 3> new_acceleration = { { 0, 0, 0 } };
	
	float new_magnitude = 0.0f;
	
	for (int i = 0; i < 3; i++) {
		new_acceleration[i] = (direction[i] * magnitude) - relative_gravity[i];
		new_magnitude += new_acceleration[i] * new_acceleration[i];
	}
	magnitude = sqrtf(new_magnitude);

	for (int i = 0; i < 3; i++) {
		direction[i] = new_acceleration[i] / magnitude;
	}
}