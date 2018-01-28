#pragma once

#include "./OPengine.h"
#include <entityx/entityx.h>

extern OPcam CAMERA;
extern OPphysXScene* PHYSX_SCENE;
extern OPphysXScene* PHYSX_TRAIN_SCENE;
extern OPphysXControllerManager* PHYSX_CONTROLLERMANAGER;
extern OPphysXControllerManager* PHYSX_TRAIN_CONTROLLERMANAGER;
extern entityx::EntityX EX;
extern OPmodel* CUBE_MODEL;
extern OPmaterial* SOLID_MATERIAL;

extern ui32 SPRITE3DSCOUNT;
extern OPtexture3D SPRITE3DS[16];

extern OPvec3 TRAIN_POSITION;
extern OPvec3 PREV_TRAIN_POSITION;
extern OPfloat TRAIN_SPEED;

extern bool PLAYERISDEAD;
extern OPscene SCENE;

extern ui64 WOBBLES[10];
extern i64 CAMSHAKE;
extern ui64 MONEY;

extern bool TRAIN_ACTIVATED;
extern bool SHOW_MENU;
extern i64 TRAINCAR_COUNT;

extern bool SHOW_TURRET_MENU;
extern entityx::Entity::Id TURRET_ID;

extern OPphysXRigidStatic* LEFT_PHYSX_PLANE;

extern ui32 MENU_SELECTION;

void GlobalsInit();
void GlobalsInitPhysics();
void GlobalsUpdate(OPtimer* timer);
void GlobalsShutdown();

void SetWalls(f32 left, f32 right);

void HitSound();
void PurchaseSound();
void DepartSound();
void CancelSound();