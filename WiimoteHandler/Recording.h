#pragma once

#include "stdafx.h"

#include "LinearAcceleration.h"
#include "RotationalMotion.h"

class Recording {
public:
	Recording();
	~Recording();
};

class RecordingFrame {
public:
	LinearAcceleration linear_acceleration;
	RotationalMotion rotational_motion;
};