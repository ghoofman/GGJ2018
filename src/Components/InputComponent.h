#pragma once

#include "./Human/include/Input/OPgamePadSystem.h"
#include "../ControlsManager.h"

struct InputComponent {
	InputComponent(i32 playerIndex);

	bool canMove = true;
	ControlsManager controls;
};