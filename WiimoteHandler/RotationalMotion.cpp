#include "RotationalMotion.h"


RotationalMotion::RotationalMotion(const int* raw_reading, const int* motion_plus_calibration, const bool* rotation_mode, long long time_duration)
{
	for (int i = 0; i < 3; i++) {
		rotation_in_radians[i] = static_cast<float>(raw_reading[i] - motion_plus_calibration[i]) * M_PI / 180.0f * time_duration * performance_freq_inv / 13.768;
		if (!rotation_mode[i]) {
			rotation_in_radians[i] *= 2000.0f / 440.0f;
		}
	}
}


RotationalMotion::~RotationalMotion()
{
}

float RotationalMotion::performance_freq_inv = 0.0f;

void RotationalMotion::PreparePerformanceCounter() {
	LARGE_INTEGER perf_freq;
	QueryPerformanceFrequency(&perf_freq);
	RotationalMotion::performance_freq_inv = 1.0f / static_cast<float>(perf_freq.QuadPart);
}