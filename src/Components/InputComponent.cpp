#include "InputComponent.h"

InputComponent::InputComponent(i32 playerIndex) {
	controls.gamePad = OPGAMEPADS.Get(playerIndex);
#ifdef ADDON_socketio
	controls.socketGamePad = OPSOCKETGAMEPADS[playerIndex];
#endif	
	switch (playerIndex) {
	case 0: {
		controls.moveForward = OPkeyboardKey::W;
		controls.moveBackward = OPkeyboardKey::S;
		controls.moveLeft = OPkeyboardKey::A;
		controls.moveRight = OPkeyboardKey::D;
		controls.grab = OPkeyboardKey::E;
		controls.pull = OPkeyboardKey::Q;
		controls.powerup = OPkeyboardKey::E;
		break;
	}
	case 1: {
		controls.moveForward = OPkeyboardKey::I;
		controls.moveBackward = OPkeyboardKey::K;
		controls.moveLeft = OPkeyboardKey::J;
		controls.moveRight = OPkeyboardKey::L;
		controls.grab = OPkeyboardKey::U;
		controls.pull = OPkeyboardKey::P;
		controls.powerup = OPkeyboardKey::Y;
		break;
	}
	}
}