#pragma once

#include "./Math/include/OPvec3.h"
#include "./Math/include/OPboundingBox3D.h"
#include "./Performance/include/OPphysXController.h"

struct PhysicsStaticComponent {
	PhysicsStaticComponent(OPvec3 position, OPvec3 rotation, OPvec3 scale, OPboundingBox3D bounds);
	PhysicsStaticComponent(OPvec3 position, OPvec3 rotation, OPvec3 scale, OPboundingBox3D bounds, OPvec3* verts, ui32 vertCount, ui32* indices, ui32 indexCount);

	OPphysXRigidStatic* physx;
	PxShape* shapes;
};