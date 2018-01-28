#pragma once

#include "./Core/include/OPtypes.h"

struct DamageComponent {
	DamageComponent() {

	}
	DamageComponent(ui64 dmg, ui64 perSecond) {
        damage = dmg;
        dps = perSecond;
        elapsedSinceLast = perSecond;
	}

	ui64 damage = 1;
    ui64 dps = 100;
    ui64 elapsedSinceLast = 0;
};