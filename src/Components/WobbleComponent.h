#pragma once

#include "entityx.h"

struct WobbleComponent {
	WobbleComponent(ui32 ind) {
        index = ind;
    }
    
    ui64 elapsed = 0;
    ui32 index;
};