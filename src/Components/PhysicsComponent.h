#pragma once

#include "./Math/include/OPvec3.h"
#include "./Performance/include/OPphysXController.h"
#include "../Globals.h"

struct PhysicsComponent {
	PhysicsComponent(OPvec3 position, OPphysXControllerManager* pxcontroller) {
		OPphysXMaterial* material = OPphysXCreateMaterial(1.0, 1.0, 1.0); // OPphysXCreateMaterial(0.8, 0.8, 0.6);
		controller = OPphysXControllerCreate(pxcontroller, material, 0.5, 0.45, NULL, NULL, NULL, NULL);
		OPphysXControllerSetPos(controller, position);
		controller->setStepOffset(0.1);
	}

	PhysicsComponent(OPvec3 position, OPfloat width, OPfloat height, OPphysXControllerManager* pxcontroller) {
		OPphysXMaterial* material = OPphysXCreateMaterial(1.0, 1.0, 1.0); // OPphysXCreateMaterial(0.8, 0.8, 0.6);
		controller = OPphysXControllerCreate(pxcontroller, material, height, width / 2.0, NULL, NULL, NULL, NULL);
		OPphysXControllerSetPos(controller, position);
		controller->setStepOffset(0.1);
	}

	OPphysXController* controller;
};