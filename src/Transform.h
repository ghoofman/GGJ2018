#pragma once

#include "./Math/include/OPvec3.h"

struct Transform {
	OPvec3 translate, rotate, scale;

	Transform() {
		translate = OPVEC3_ZERO;
		rotate = OPVEC3_ZERO;
		scale = OPVEC3_ONE;
	}

	Transform(OPvec3 t, OPvec3 r, OPvec3 s) {
		translate = t;
		rotate = r;
		scale = s;
	}
};
