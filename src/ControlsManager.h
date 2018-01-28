#pragma once

struct ControlsManager;

#include "./OPengine.h"
#ifdef ADDON_socketio
#include "OPsocketGamePadSystem.h"
#endif

struct ControlsManager {
	OPgamePad* gamePad;
#ifdef ADDON_socketio
	OPsocketGamePad* socketGamePad;
#endif
	OPkeyboardKey
		moveForward = OPkeyboardKey::W,
		moveBackward = OPkeyboardKey::S,
		moveLeft = OPkeyboardKey::A,
		moveRight = OPkeyboardKey::D,
		grab = OPkeyboardKey::E,
		pull = OPkeyboardKey::Q,
		powerup = OPkeyboardKey::R;

	inline OPvec2 Movement() {
		return		
#ifdef ADDON_socketio
		socketGamePad->LeftThumb() +
#endif
		 gamePad->LeftThumb() +
			OPvec2(
			(OPfloat)(OPKEYBOARD.IsDown(moveRight) - OPKEYBOARD.IsDown(moveLeft)),
				(OPfloat)(OPKEYBOARD.IsDown(moveForward) - OPKEYBOARD.IsDown(moveBackward)));
	}
	
	inline bool Running() {
		return OPvec2Len(Movement()) > 0;
	}

	inline bool Grab() {
		return
#ifdef ADDON_socketio
		socketGamePad->WasPressed(OPgamePadButton::A) ||
#endif	
		gamePad->WasPressed(OPgamePadButton::A) ||
			OPKEYBOARD.WasPressed(grab);
	}

	inline bool GrabDown() {
		return
#ifdef ADDON_socketio
		socketGamePad->IsDown(OPgamePadButton::A) ||
#endif	
		gamePad->IsDown(OPgamePadButton::A) ||
			OPKEYBOARD.IsDown(grab);
	}

	inline bool PullDown() {
		return
#ifdef ADDON_socketio
		socketGamePad->IsDown(OPgamePadButton::B) ||
#endif	
		 gamePad->IsDown(OPgamePadButton::B) ||
			OPKEYBOARD.IsDown(pull);
	}

	inline bool PowerUpWasPressed() {
		return
#ifdef ADDON_socketio
		socketGamePad->WasPressed(OPgamePadButton::X) ||
#endif	
		 gamePad->WasPressed(OPgamePadButton::X) ||
			OPKEYBOARD.WasPressed(powerup);
	}
};
