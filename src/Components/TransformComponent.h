#pragma once

#include "./Math/include/OPvec3.h"
#include "./Math/include/OPquat.h"
#include "./Math/include/OPboundingBox3D.h"

struct TransformComponent {
	TransformComponent(OPvec3 _pos, OPvec3 _scl, OPvec3 _rot, OPboundingBox3D _bounds, OPvec3 _boundingBoxPos) {
		pos = _pos;
		scl = _scl;
		q = OPquat::CreateRot(OPvec3(0, 1, 0), _rot.y) * OPquat::CreateRot(OPvec3(1, 0, 0), _rot.x) * OPquat::CreateRot(OPvec3(0, 0, 1), _rot.z);

		prevPos = pos;
		prevScl = scl;
		prevQ = q;

		bounds = _bounds;
		boundingBoxPos = _boundingBoxPos;

		startingPos = pos;
	}

	OPvec3 prevPos, prevScl;
	OPvec3 pos, scl;
	OPquat q, prevQ;
	OPboundingBox3D bounds;
	OPvec3 boundingBoxPos;
	OPvec3 baseRotation = OPVEC3_ZERO;
	OPvec3 startingPos;
};