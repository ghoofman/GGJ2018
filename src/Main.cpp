//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
#include "./OPengine.h"
#include "Main.h"
#include "GameState.h"
#include "SceneLoader.h"

//////////////////////////////////////
// Application Methods
//////////////////////////////////////

OPwindow mainWindow;

void ApplicationInit() {

	OPCMAN.Init(OPIFEX_ASSETS);
	OPloadersAddDefault();
	OPscriptAddLoader();
	SceneAddLoader();
	OPskeletonAddLoader();
	OPskeletonAnimationAddLoader();

	OPrenderSetup();

	OPwindowSystemInit();
	

//#ifndef _DEBUG
//	OPmonitor primary = OPmonitor::GetAll().primary;
//	mainWindow.Init(&primary, OPwindowParameters("Main Window", false, primary.VideoModeCurrent.Width, primary.VideoModeCurrent.Height));	
//#else
	mainWindow.Init(NULL, OPwindowParameters("Main Window", false, 1280, 720));
	//mainWindow.Init(NULL, OPwindowParameters("Main Window", false, 2560, 1300));
//#endif
	OPrenderInit(&mainWindow);


	// Sound System initialize
	OPfmodInit();

	// Physics Setup
	OPphysXInit();

	OPGAMEPADS.SetDeadzones(0.2f);

	
	
	OPfontSystemLoadEffects();

	OPgameState::Change(&GS_GAME_STATE);
}

OPint ApplicationUpdate(OPtimer* timer) {
	if (mainWindow.Update()) {
		// Window received an exit request
		return 1;
	}

	OPinputSystemUpdate(timer);
	if (OPKEYBOARD.WasPressed(OPkeyboardKey::ESCAPE)) return 1;

	return ActiveState->Update(timer);
}

void ApplicationRender(OPfloat delta) {
	ActiveState->Render(delta);
}

void ApplicationDestroy() {

}

void ApplicationSetup() {
	OPinitialize = ApplicationInit;
	OPupdate = ApplicationUpdate;
	OPrender = ApplicationRender;
	OPdestroy = ApplicationDestroy;
}

//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
OP_MAIN_START

	OPLOGLEVEL = (ui32)OPlogLevel::VERBOSE;
	OPlog("Starting up OPifex Engine");

	ApplicationSetup();

	OP_MAIN_RUN
	//OP_MAIN_RUN_STEPPED

OP_MAIN_END
