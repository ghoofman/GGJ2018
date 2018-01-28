#pragma once

#include "./Math/include/OPvec3.h"

struct DirectionComponent {
	DirectionComponent(OPvec3 accel, OPvec3 vel) : acceleration(accel), velocity(vel) {
		facing = OPvec3Norm(vel);
		additionalOffset = OPVEC3_ZERO;
	}
	DirectionComponent(OPvec3 accel, OPvec3 vel, OPfloat max) : acceleration(accel), velocity(vel) {
		facing = OPvec3Norm(vel);
		additionalOffset = OPVEC3_ZERO;
        maxSpeed = max;
	}

	OPvec3 acceleration, velocity, facing;
	OPfloat speedVariant = 1.0;
    OPfloat maxSpeed = 0.3f;
	OPvec3 additionalOffset;
};