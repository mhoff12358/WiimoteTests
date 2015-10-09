#include "LocationAndVelocity.h"


LocationAndVelocity::LocationAndVelocity() : location({ { 0, 0, 0 } }), velocity({ { 0, 0, 0 } })
{
}


LocationAndVelocity::~LocationAndVelocity()
{
}

void LocationAndVelocity::UpdateWithAcceleration(const LinearAcceleration& acceleration, float update_duration) {
	for (int i = 0; i < 3; i++) {
		velocity[i] += acceleration.direction[i] * acceleration.magnitude * update_duration * 1000.0f;
	}
}

void LocationAndVelocity::ApplyVelocityDecay(float velocity_decay) {
	for (int i = 0; i < 3; i++) {
		velocity[i] *= velocity_decay;
	}
}