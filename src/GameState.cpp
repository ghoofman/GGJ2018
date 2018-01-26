#include "./GameState.h"


void ExampleState::Init(OPgameState* last) {
	
}


OPint ExampleState::Update(OPtimer* time) {
	return false;
}

void ExampleState::Render(OPfloat delta) {
	OPrenderClear(0, 0, 0, 1);
	OPrenderPresent();
}

OPint ExampleState::Exit(OPgameState* next) {
	return 0;
}

ExampleState GS_EXAMPLE;
