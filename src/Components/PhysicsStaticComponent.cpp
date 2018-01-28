#include "PhysicsStaticComponent.h"
#include "../Globals.h"

PhysicsStaticComponent::PhysicsStaticComponent(OPvec3 position, OPvec3 rotation, OPvec3 scale, OPboundingBox3D bounds) {
	OPphysXMaterial* material = OPphysXCreateMaterial(0.8, 0.8, 0.2);
	physx = OPphysXSceneCreateStatic(PHYSX_SCENE, position);

	bounds.max *= scale;
	bounds.min *= scale;

	shapes = OPphysXAddBoxShape(physx, material, (bounds.max - bounds.min) / 2.0);
	OPvec3 centerPoint = bounds.max + bounds.min;
	shapes->setLocalPose(PxTransform(PxVec3(centerPoint.x / 2.0, centerPoint.y / 2.0, centerPoint.z / 2.0), PxQuat(rotation.y, PxVec3(0, 1, 0))));
	OPphysXSceneAddActor(PHYSX_SCENE, physx);
}

PhysicsStaticComponent::PhysicsStaticComponent(OPvec3 position, OPvec3 rotation, OPvec3 scale, OPboundingBox3D bounds, OPvec3* verts, ui32 vertCount, ui32* indices, ui32 indexCount) {
	OPphysXMaterial* material = OPphysXCreateMaterial(0.8, 0.8, 0.2);
	physx = OPphysXSceneCreateStatic(PHYSX_SCENE, position);

	PxTriangleMesh* pxMesh = OPphysXCreateTriangleMesh(
		vertCount,
		(PxVec3*)verts,
		sizeof(f32) * 3,
		indexCount / 3,
		(PxU32*)indices,
		sizeof(ui32));

	shapes = OPphysXAddTriangleMeshShape(physx, material, pxMesh);
	shapes->setLocalPose(PxTransform(PxQuat(rotation.y, PxVec3(0, 1, 0)) * PxQuat(rotation.x, PxVec3(1, 0, 0)) * PxQuat(rotation.z, PxVec3(0, 0, 1))));
	OPphysXSceneAddActor(PHYSX_SCENE, physx);
}