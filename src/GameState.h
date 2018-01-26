#pragma once

#include "./OPengine.h"

class ExampleState : public OPgameState {
    void Init(OPgameState* last);
    OPint Update(OPtimer* time);
    void Render(OPfloat delta);
    OPint Exit(OPgameState* next);
};

extern ExampleState GS_EXAMPLE;
