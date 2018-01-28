#pragma once

#include "./Core/include/OPtypes.h"

struct HealthComponent {
	HealthComponent() {
        health = 5;
    }
	HealthComponent(i64 h) {
        health = h;
    }

    i64 health;
};