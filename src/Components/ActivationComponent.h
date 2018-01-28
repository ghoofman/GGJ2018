#pragma once

#include "./Core/include/OPtypes.h"
#include "entityx.h"

typedef void(*ActivationFn)(entityx::Entity*, void*);

struct ActivationComponent {
	ActivationComponent(ActivationFn _activate, void* _data, const OPchar* n) {
		activateFn = _activate;
		data = _data;
        name = n;
	}

	ActivationFn activateFn = NULL;
	void* data = NULL;
    const OPchar* name;
};