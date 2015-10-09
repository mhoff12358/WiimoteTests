#pragma once

#include <array>

#include "LinearAcceleration.h"

class LocationAndVelocity
{
public:
	LocationAndVelocity();
	~LocationAndVelocity();

	void UpdateWithAcceleration(const LinearAcceleration& acceleration, float update_duration);
	void ApplyVelocityDecay(float velocity_decay);

	std::array<float, 3> location;
	std::array<float, 3> velocity;
};

