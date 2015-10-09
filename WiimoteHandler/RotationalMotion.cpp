#include "RotationalMotion.h"


RotationalMotion::RotationalMotion(const int* raw_reading, const int* motion_plus_calibration, const bool* rotation_mode, float duration_in_sec)
{
	for (int i = 0; i < 3; i++) {
		rotation_in_radians[i] = static_cast<float>(raw_reading[i] - motion_plus_calibration[i]) * M_PI / 180.0f * duration_in_sec / 13.768;
		if (!rotation_mode[i]) {
			rotation_in_radians[i] *= 2000.0f / 440.0f;
		}
	}
}


RotationalMotion::~RotationalMotion()
{
}
