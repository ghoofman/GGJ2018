#include "Globals.h"
#include "Jukebox.h"
#include <entityx/entityx.h>


#ifdef ADDON_socketio
#include "OPsocketGamePadSystem.h"
#endif

OPcam CAMERA;
OPphysXScene* PHYSX_SCENE;
OPphysXScene* PHYSX_TRAIN_SCENE;

OPphysXControllerManager* PHYSX_CONTROLLERMANAGER;
OPphysXControllerManager* PHYSX_TRAIN_CONTROLLERMANAGER;
bool _GLOBALS_INITIALIZED = false;
OPscene SCENE;
i64 TRAINCAR_COUNT;

entityx::EntityX EX;

OPmodel* CUBE_MODEL; 
OPmaterial* SOLID_MATERIAL;

ui32 SPRITE3DSCOUNT = 0;
OPtexture3D SPRITE3DS[16];

ui64 WOBBLES[10];

i64 CAMSHAKE = 0;

Jukebox jukebox;

#define SONG_COUNT 3
OPchar* bgs[] = {
    "Audio/bg1.mp3",
    "Audio/bg2.mp3",
    "Audio/bg3.mp3"
};

OPvec3 TRAIN_POSITION;
OPvec3 PREV_TRAIN_POSITION;
OPfloat TRAIN_SPEED;
bool PLAYERISDEAD;
ui64 MONEY;
bool TRAIN_ACTIVATED;
bool SHOW_MENU;

ui32 MENU_SELECTION;

bool SHOW_TURRET_MENU;
entityx::Entity::Id TURRET_ID;

void GlobalsInitPhysics() {
	PHYSX_SCENE = OPphysXSceneCreate(OPvec3Create(0, -9.8, 0), nullptr, nullptr, nullptr);
	PHYSX_CONTROLLERMANAGER = OPphysXControllerCreateManager(PHYSX_SCENE);
	PHYSX_CONTROLLERMANAGER->setPreventVerticalSlidingAgainstCeiling(true);

	OPphysXMaterial* material = OPphysXCreateMaterial(0.8, 0.8, 0.1);
	
    {
        PxTransform transform;
        OPphysXRigidStatic* plane;
        transform = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
        plane = OPphysXSceneCreateStatic(PHYSX_SCENE, transform);
        OPphysXAddPlaneShape(plane, material);
        OPphysXSceneAddActor(PHYSX_SCENE, plane);
    }
	//OPphysXSceneAddWalls(PHYSX_SCENE, -25, 25, 25, -25, material, true);
	OPphysXSceneUpdate(PHYSX_SCENE, 16);


    PHYSX_TRAIN_SCENE = OPphysXSceneCreate(OPvec3Create(0, -9.8, 0), nullptr, nullptr, nullptr);
	PHYSX_TRAIN_CONTROLLERMANAGER = OPphysXControllerCreateManager(PHYSX_TRAIN_SCENE);
	PHYSX_TRAIN_CONTROLLERMANAGER->setPreventVerticalSlidingAgainstCeiling(true);

	// OPphysXSceneAddWalls(PHYSX_TRAIN_SCENE, -16, 1, 1.5, -1.5, material, true);
	// OPphysXSceneUpdate(PHYSX_TRAIN_SCENE, 16);
}


OPphysXRigidStatic* LEFT_PHYSX_PLANE;

void SetWalls(f32 left, f32 right) {
	OPphysXMaterial* material = OPphysXCreateMaterial(0.8, 0.8, 0.1);
	// OPphysXSceneAddWalls(PHYSX_TRAIN_SCENE, left, right, 1.5, -1.5, material, true);

	PxTransform transform;
	OPphysXRigidStatic* plane;

    f32 backward = -1.5f;
    f32 forward = 1.5f;

	// if(addFloor)
	{
		transform = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
		plane = OPphysXSceneCreateStatic(PHYSX_TRAIN_SCENE, transform);
		OPphysXAddPlaneShape(plane, material);
		OPphysXSceneAddActor(PHYSX_TRAIN_SCENE, plane);
	}

	{
		transform = PxTransform(PxVec3(left, 0.0f, 0.0f), PxQuat(0, PxVec3(1.0f, 0.0f, 0.0f)));
		LEFT_PHYSX_PLANE = OPphysXSceneCreateStatic(PHYSX_TRAIN_SCENE, transform);
		OPphysXAddPlaneShape(LEFT_PHYSX_PLANE, material);
		OPphysXSceneAddActor(PHYSX_TRAIN_SCENE, LEFT_PHYSX_PLANE);
	}

	{
		transform = PxTransform(PxVec3(right, 0.0f, 0.0f), PxQuat(PxPi, PxVec3(0.0f, 1.0f, 0.0f)));
		plane = OPphysXSceneCreateStatic(PHYSX_TRAIN_SCENE, transform);
		OPphysXAddPlaneShape(plane, material);
		OPphysXSceneAddActor(PHYSX_TRAIN_SCENE, plane);
	}

	{
		transform = PxTransform(PxVec3(0.0f, 0.0f, forward), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
		plane = OPphysXSceneCreateStatic(PHYSX_TRAIN_SCENE, transform);
		OPphysXAddPlaneShape(plane, material);
		OPphysXSceneAddActor(PHYSX_TRAIN_SCENE, plane);
	}

	{
		transform = PxTransform(PxVec3(0.0f, 0.0f, backward), PxQuat(-PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));
		plane = OPphysXSceneCreateStatic(PHYSX_TRAIN_SCENE, transform);
		OPphysXAddPlaneShape(plane, material);
		OPphysXSceneAddActor(PHYSX_TRAIN_SCENE, plane);
	}


	OPphysXSceneUpdate(PHYSX_TRAIN_SCENE, 16);
}

void GlobalsInit() {
	if (_GLOBALS_INITIALIZED) return;
	_GLOBALS_INITIALIZED = true;

    WOBBLES[0] = 0;
    for(ui32 i = 1; i < 10; i++) { // Skip the main train wobble
        WOBBLES[i] = OPrandom() * 5000;
    }
	TRAIN_SPEED = 0;
    PLAYERISDEAD = false;
    MONEY = 0;
    TRAIN_ACTIVATED = false;
    SHOW_MENU = false;
    SHOW_TURRET_MENU = false;
    MENU_SELECTION = 0;
    TRAINCAR_COUNT = 0;

	CAMERA.SetPerspective(OPvec3Create(-15, 15, 15), OPVEC3_UP);
	GlobalsInitPhysics();

	OPphysXDebugger("127.0.0.1");

	jukebox.Init();
	for (ui32 i = 0; i < SONG_COUNT; i++) {
		jukebox.Add(bgs[i]);
	}
	jukebox.Play();

	CUBE_MODEL = OPcubeCreate(OPVEC3_ZERO, OPVEC3_ONE);
	OPeffect* solidMaterialEffect = OPeffect::Create(OPshader::Load("solid.vert"), OPshader::Load("solid.frag"));
	SOLID_MATERIAL = OPmaterial::Create(solidMaterialEffect);

    TRAIN_POSITION = PREV_TRAIN_POSITION = OPvec3(0, 1.4f, 0);

    SetWalls(-5.0f, 1.0f);

#ifdef ADDON_socketio
	OPSOCKETGAMEPADS.Init();
	OPSOCKETGAMEPADS.SetDeadzones(0.2f);
#endif
}

void GlobalsUpdate(OPtimer* timer) {
	jukebox.Update();
    for(ui32 i = 0; i < 10; i++) {
        WOBBLES[i] += timer->Elapsed;
    }
    if(CAMSHAKE > 0) {
        CAMSHAKE -= timer->Elapsed;
        if(CAMSHAKE < 0) {
            CAMSHAKE = 0;
        }
    }
#ifdef ADDON_socketio
	OPSOCKETGAMEPADS.Update(timer);
#endif
}


void HitSound() {
	OPfmodSound* s = OPfmodLoad("Audio/Hit_Hurt5.wav");
	OPfmodPlay(s);
}

void PurchaseSound() {
	OPfmodSound* s = OPfmodLoad("Audio/Pickup_Coin6.wav");
	OPfmodPlay(s);
}

void DepartSound() {
	OPfmodSound* s = OPfmodLoad("Audio/Pickup_Coin6.wav");
	OPfmodPlay(s);
}

void CancelSound() {
	OPfmodSound* s = OPfmodLoad("Audio/Blip_Select4.wav");
	OPfmodPlay(s);
}

void GlobalsShutdown() {

	PHYSX_CONTROLLERMANAGER->release();
	OPphysXSceneDestroy(PHYSX_SCENE);
	jukebox.Destroy();
	OPtexture3D::Shutdown();
	OPparticleSystem::Shutdown();

	CUBE_MODEL->Destroy();
	OPfree(CUBE_MODEL);

	SOLID_MATERIAL->effect->Destroy();
	OPfree(SOLID_MATERIAL->effect);
	SOLID_MATERIAL->Destroy();
	OPfree(SOLID_MATERIAL);
}