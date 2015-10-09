#pragma once

#include "stdafx.h"
#include <iostream>

#include <array>
#define _USE_MATH_DEFINES
#include <math.h>

class RotationalMotion
{
public:
	RotationalMotion(const int* raw_reading, const int* motion_plus_calibration, const bool* rotation_mode, float duration_in_sec);
	~RotationalMotion();

	std::array<float, 3> rotation_in_radians;
};

