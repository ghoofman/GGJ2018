#pragma once

#include "./Core/include/OPtypes.h"
#include "entityx.h"

typedef void(*AIUpdateFn)(entityx::Entity*, void*, f32 delta);

struct AIComponent {
	AIComponent(AIUpdateFn _update, void* _data) {
		updateFn = _update;
		data = _data;
	}

	void Update(entityx::Entity*entity, f32 delta) {
		updateFn(entity, data, delta);
	}

	AIUpdateFn updateFn = NULL;
	void* data = NULL;
	entityx::Entity::Id targetId;
    bool hasTarget = false;
};