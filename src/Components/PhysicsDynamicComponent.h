#pragma once

#include "../Globals.h"
#include "./Performance/include/OPphysXController.h"

struct PhysicsDynamicComponent {
	PhysicsDynamicComponent(OPvec3 position, OPvec3 rotation, OPvec3 scale, OPboundingBox3D bounds) {
		physx = OPphysXSceneCreateDynamic(PHYSX_SCENE, position);
	}

	OPphysXRigidDynamic* physx;
};