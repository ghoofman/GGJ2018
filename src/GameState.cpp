#include "./GameState.h"

#include "Globals.h"

#include "Components/TransformComponent.h"
#include "Components/DirectionComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/PhysicsStaticComponent.h"
#include "Components/InputComponent.h"
#include "Components/RenderComponent.h"
#include "Components/FollowComponent.h"
#include "Components/OffsetComponent.h"
#include "Components/TrainComponent.h"
#include "Components/TrainCarComponent.h"
#include "Components/AIComponent.h"
#include "Components/ActivationComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/HealthComponent.h"
#include "Components/DamageComponent.h"
#include "Components/WobbleComponent.h"
#include "Components/AnimationComponent.h"

#include "Systems/InputSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/WeaponSystem.h"
// #include "Systems/ActivationSystem.h"
// #include "Systems/AnimationSystem.h"
// #include "Systems/AttackSystem.h"

// Transmission

bool Arriving = false;

void GameState::InitScene() {
	SCENE.Init(&rendererForward, 1000, 1000);
	SCENE.camera = &CAMERA;

	OPfloat shadowCameraSize = 32.0f;
	SCENE.shadowCamera->SetOrtho(OPvec3(30, 35, 45), OPVEC3_ZERO, OPVEC3_UP, 15.0f, 200.0f, -shadowCameraSize, shadowCameraSize, -shadowCameraSize, shadowCameraSize);



	SOLID_MATERIAL->paramIndex = 0;
	SOLID_MATERIAL->AddParam("uViewShadow", &SCENE.shadowCamera->view);
	SOLID_MATERIAL->AddParam("uProjShadow", &SCENE.shadowCamera->proj);
	SOLID_MATERIAL->AddParam("uShadow", &rendererForward.shadowPass.depthBuffer.texture, 1);
	SOLID_MATERIAL->AddParam("uLightPos", &SCENE.shadowCamera->pos);
	SOLID_MATERIAL->AddParam("uViewPos", &SCENE.camera->pos);
}

void TrainActivate(entityx::Entity* entity, void* data) {
	OPlogErr("Activated!");

	SHOW_MENU = true;
	MENU_SELECTION = 0;

	// entityx::ComponentHandle<TrainComponent> train;
	// for (entityx::Entity entityOther : EX.entities.entities_with_components(train)) {
	// 	train->activated = !train->activated;
	// 	TRAIN_ACTIVATED = true;
	// }
}

void WeaponPlateActivate(entityx::Entity* entity, void* data) {
	OPlogErr("Activated!");

	SHOW_TURRET_MENU = true;
	TURRET_ID = entity->id();
	MENU_SELECTION = 0;

}

void EnemyUpdate(entityx::Entity* entity, void* data, f32 delta) {

	entityx::ComponentHandle<DirectionComponent> dirComponent = entity->component<DirectionComponent>();
	entityx::ComponentHandle<TransformComponent> transformComponent = entity->component<TransformComponent>();
	entityx::ComponentHandle<AIComponent> aiComponent = entity->component<AIComponent>();
	entityx::ComponentHandle<DamageComponent> dmgComponent = entity->component<DamageComponent>();

	if(aiComponent->hasTarget) {
		// if in range, attack
		
		if(!EX.entities.valid(aiComponent->targetId)) {
			aiComponent->hasTarget = false;
		} else {
			entityx::Entity e = EX.entities.get(aiComponent->targetId);
			entityx::ComponentHandle<TransformComponent> transformTargetComponent = e.component<TransformComponent>();
			entityx::ComponentHandle<HealthComponent> healthTargetComponent = e.component<HealthComponent>();

			OPvec3 playerPos = transformTargetComponent->pos;
			OPvec3 enemyPos = transformComponent->pos;

			if(OPabs(OPvec3Len(enemyPos - playerPos)) < 1.5) {
				dmgComponent->elapsedSinceLast += delta;
				if(dmgComponent->elapsedSinceLast > dmgComponent->dps) {
					dmgComponent->elapsedSinceLast -= dmgComponent->dps;

					healthTargetComponent->health -= dmgComponent->damage;

					if(healthTargetComponent->health <= 0) {
						aiComponent->hasTarget = false;
					}

					CAMSHAKE = 150;

					OPlogErr("Dealing Damage: %d | %d", dmgComponent->damage, healthTargetComponent->health);
				}
			}

			// Enemy is on the train
			// Go toward the enemy
			if(entity->has_component<OffsetComponent>()) {
				if(OPabs(OPvec3Len(enemyPos - playerPos)) < 1.5) {
					// just stay put
					dirComponent->acceleration = OPvec3(0, 0, 0);
				} else if(enemyPos.x > playerPos.x) {
					// enemy is to the right
					dirComponent->acceleration = OPvec3(-3, 0, 0);
					dirComponent->facing = OPvec3(-1, 0, 0);
				} else {
					// enemy is to the left
					dirComponent->acceleration = OPvec3(3, 0, 0);
					dirComponent->facing = OPvec3(1, 0, 0);
				}
			}
		}

	} else {
		// attempt to find target

		entityx::ComponentHandle<HealthComponent> healthComp;
		entityx::ComponentHandle<InputComponent> inputComp;
		for (entityx::Entity entityOther : EX.entities.entities_with_components(healthComp, inputComp)) {
			if(healthComp->health > 0) {
				aiComponent->targetId = entityOther.id();
				aiComponent->hasTarget = true;
			}
			break;
		}

	}


	if(!entity->has_component<OffsetComponent>()) {
		OPvec3 backOfTrainPos = (TRAIN_POSITION - OPvec3(((TRAINCAR_COUNT - 1) * 9.5f), 0, -2.0f));
		OPvec3 diff = transformComponent->pos - backOfTrainPos;

	
		OPfloat angle = OPvec3AngleToTarget(backOfTrainPos, OPvec3(1,0,0), transformComponent->pos);
		if(OPabs(diff.z) < 0.04 ) {
			angle = 0;
		}
		//OPfloat angle = OPvec3Angle(OPvec3(1,0,0), OPvec3Norm(diff));

		dirComponent->acceleration = OPvec3Norm(OPvec3(3, 0, angle * 1.75f));
		// dirComponent->acceleration *= TRAIN_SPEED;
		dirComponent->maxSpeed = TRAIN_SPEED * 0.5f;

		dirComponent->facing = OPvec3Norm(dirComponent->acceleration);
	}
	
	
	OPboundingBox3D bounds;
	bounds.min = transformComponent->bounds.min;
	bounds.max = transformComponent->bounds.max;
	bounds.min = bounds.min + transformComponent->pos;
	bounds.max = bounds.max + transformComponent->pos;


	entityx::ComponentHandle<TransformComponent> trainCarTransform;
	entityx::ComponentHandle<TrainCarComponent> trainCarComponent;

	for (entityx::Entity entityOther : EX.entities.entities_with_components(trainCarComponent, trainCarTransform)) {

		if(!entity->has_component<OffsetComponent>()) {
			OPboundingBox3D bounds2 = trainCarTransform->bounds;
			bounds2.min = trainCarTransform->bounds.min * trainCarTransform->scl;
			bounds2.max = (trainCarTransform->bounds.min + OPvec3(0.5, 5, 3.2f)) * trainCarTransform->scl;
			bounds2.min = bounds2.min + trainCarTransform->pos + trainCarTransform->boundingBoxPos;
			bounds2.max = (trainCarTransform->bounds.min + OPvec3(0.5, 5, 3.2f)) + trainCarTransform->pos + trainCarTransform->boundingBoxPos;

			OPintersectionType collType = bounds.Collision(bounds2);

			// if enemy is in bounds of train car, transition
			if (collType != OPintersectionType::NONE) {		


				entityx::ComponentHandle<DirectionComponent> dirComponent = entity->component<DirectionComponent>();
				dirComponent->maxSpeed = 0.1;

				// // Kill previous physics controller
				entityx::ComponentHandle<PhysicsComponent> physicsComponent = entity->component<PhysicsComponent>();
				if(physicsComponent != NULL) {
					physicsComponent->controller->release();
					entity->remove<PhysicsComponent>();

					OPvec3 p = OPvec3(bounds2.min.x - TRAIN_POSITION.x, 0, transformComponent->pos.z);

					entity->assign<PhysicsComponent>(p, PHYSX_TRAIN_CONTROLLERMANAGER);
				}

				// // Add new physics controller
				entity->assign<OffsetComponent>();
			}
		}
	}

	entityx::ComponentHandle<TransformComponent> trainTransform;
	entityx::ComponentHandle<TrainComponent> trainComponent;

	for (entityx::Entity entityOther : EX.entities.entities_with_components(trainComponent, trainTransform)) {

		if(!entity->has_component<OffsetComponent>()) {
			OPboundingBox3D bounds2 = trainTransform->bounds;
			bounds2.min = trainTransform->bounds.min * trainTransform->scl;
			bounds2.max = (trainTransform->bounds.min + OPvec3(0.5, 5, 3.2f)) * trainTransform->scl;
			bounds2.min = bounds2.min + trainTransform->pos + trainTransform->boundingBoxPos;
			bounds2.max = (trainTransform->bounds.min + OPvec3(0.5, 5, 3.2f)) + trainTransform->pos + trainTransform->boundingBoxPos;

			OPintersectionType collType = bounds.Collision(bounds2);

			// if enemy is in bounds of train car, transition
			if (collType != OPintersectionType::NONE) {		


				entityx::ComponentHandle<DirectionComponent> dirComponent = entity->component<DirectionComponent>();
				dirComponent->maxSpeed = 0.1;

				// // Kill previous physics controller
				entityx::ComponentHandle<PhysicsComponent> physicsComponent = entity->component<PhysicsComponent>();
				if(physicsComponent != NULL) {
					physicsComponent->controller->release();
					entity->remove<PhysicsComponent>();

					OPvec3 p = OPvec3(bounds2.min.x - TRAIN_POSITION.x, 0, trainTransform->pos.z);

					entity->assign<PhysicsComponent>(p, PHYSX_TRAIN_CONTROLLERMANAGER);
				}

				// // Add new physics controller
				entity->assign<OffsetComponent>();
			} else {
				OPlogErr("Not touching train");
			}
		}
	}
}

void GameState::Init(OPgameState* last) {
	GlobalsInit();

	InitScene();

	OPtexture* tex = (OPtexture*)OPCMAN.LoadGet("Screen.png");
	screenTex = OPtexture2DCreate(tex, NULL);
	screenTex->Position.x = 0.5f;// (OPfloat)OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2.0;
	screenTex->Position.y = 0.5f; // -(OPfloat)OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2.0;


	EX.systems.add<InputSystem>();
	EX.systems.add<MovementSystem>();
	EX.systems.add<RenderSystem>();
	EX.systems.add<PhysicsSystem>();
	EX.systems.add<WeaponSystem>();
	// EX.systems.add<ActivationSystem>();
	// EX.systems.add<AttackSystem>();
	// EX.systems.add<AnimationSystem>();
	EX.systems.configure();



	
	FontManager = OPfontManager::Create("Pixel.opf", NULL, 0);



	MONEY = 100;

	current_wave = -1;
	time_until_next_wave = 10000;
	show_wave = 0;
	wave_count = 1;

    waves[0] = 1;
	waves_hard[0] = 0;
    waves_time[0] = 15000;
	
    waves[1] = 3;
	waves_hard[1] = 0;
    waves_time[1] = 19000;
	
    waves[2] = 9;
	waves_hard[2] = 1;
    waves_time[2] = 21000;
	
    waves[3] = 18;
	waves_hard[3] = 2;
    waves_time[3] = 23000;
	
    waves[4] = 25;
	waves_hard[4] = 3;
    waves_time[4] = 25000;



	// Example Player
	{
		OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet("Person.opm"), OPrendererEntityDesc(false, true, true, false));
		// OPrendererEntity* re = SCENE.Add(CUBE_MODEL, OPrendererEntityDesc(false, true, true, false));
		// {
		// 	re->material->Destroy();
		// 	OPfree(re->material);
		// 	re->material = SOLID_MATERIAL->CreateInstances(CUBE_MODEL, false);
		// }

		entityx::Entity e = EX.entities.create();

		OPrendererEntity* renderEntity = re;

		e.assign<TransformComponent>(OPVEC3_ZERO, OPvec3(1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-1), OPvec3(1)), OPVEC3_ZERO);
		e.assign<DirectionComponent>(OPvec3(0, 0, 0), OPvec3(0, -1.0, 0), 0.05f);
		e.assign<RenderComponent>(renderEntity);
		e.assign<PhysicsComponent>(OPVEC3_ZERO, PHYSX_TRAIN_CONTROLLERMANAGER);
		e.assign<InputComponent>(0);
		e.assign<FollowComponent>();
		e.assign<OffsetComponent>();
		e.assign<HealthComponent>();
	}

	// Example Train
	// {
	// 	OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet("TrainEngineEmpty.opm"), OPrendererEntityDesc(false, true, true, false));
	// 	// OPrendererEntity* re = SCENE.Add(CUBE_MODEL, OPrendererEntityDesc(false, true, true, false));
	// 	// {
	// 	// 	re->material->Destroy();
	// 	// 	OPfree(re->material);
	// 	// 	re->material = SOLID_MATERIAL->CreateInstances(CUBE_MODEL, false);
	// 	// }

	// 	entityx::Entity e = EX.entities.create();

	// 	OPrendererEntity* renderEntity = re;

	// 	e.assign<TransformComponent>(OPVEC3_ZERO, OPvec3(1,1,1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-1), OPvec3(1)), OPVEC3_ZERO);
	// 	e.assign<DirectionComponent>(OPvec3(0, 0, 0), OPvec3(0, -1.0, 0));
	// 	e.assign<RenderComponent>(renderEntity);
	// 	e.assign<PhysicsComponent>(OPVEC3_ZERO, PHYSX_CONTROLLERMANAGER);
	// 	e.assign<InputComponent>(1);
	// 	e.assign<TrainComponent>();
	// }


	TrainStation = SCENE.Add((OPmodel*)OPCMAN.LoadGet("TrainStation.opm"), OPrendererEntityDesc(false, true, true, false));


	// Example Track
	{
		for(ui32 i = 0; i < TRACK_COUNT; i++) {
		
			f32 halfWay = TRACK_COUNT;


			tracks[i] = SCENE.Add((OPmodel*)OPCMAN.LoadGet("TrainTrack.opm"), OPrendererEntityDesc(false, true, true, false));
			OPvec3 pos = OPvec3Create(-halfWay + (i * 2.0f), -0.4f, 0.0f);
			track_pos[i] = pos;
			tracks[i]->world.SetTranslate(pos);
		}
		
		// entityx::Entity e = EX.entities.create();
		// OPrendererEntity* renderEntity = re;

		// e.assign<TransformComponent>(OPvec3(-11, -0.8f, 0), OPvec3(10, 0.5, 3), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-1), OPvec3(1)), OPVEC3_ZERO);
		// e.assign<RenderComponent>(renderEntity);
	}


	// Example Rocks
	{
		for(ui32 i = 0; i < ROCK_COUNT; i++) {
			
			f32 area = 120.0f;
			f32 halfWay = area / 2.0f;

			OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet("Rock.opm"), OPrendererEntityDesc(false, true, true, false));
			// OPrendererEntity* re = SCENE.Add(CUBE_MODEL, OPrendererEntityDesc(false, true, true, false));
			// {
			// 	re->material->Destroy();
			// 	OPfree(re->material);
			// 	re->material = SOLID_MATERIAL->CreateInstances(CUBE_MODEL, false);
			// }

			rocks[i] = re;
			OPvec3 pos = OPvec3Create(-halfWay + (OPrandom() * area), -0.4f, 5 + OPrandom() * 12.0f);
			if(i % 2 == 0) {
				pos = OPvec3Create(-halfWay + (OPrandom() * area), -0.4f, -5 - OPrandom() * 12.0f);
			}
			rock_pos[i] = pos;

			rocks[i]->world.SetTranslate(pos)->RotY(OPrandom() * OPpi);
		}
		
		// entityx::Entity e = EX.entities.create();
		// OPrendererEntity* renderEntity = re;

		// e.assign<TransformComponent>(OPvec3(-11, -0.8f, 0), OPvec3(10, 0.5, 3), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-1), OPvec3(1)), OPVEC3_ZERO);
		// e.assign<RenderComponent>(renderEntity);
	}

	{
		// Train
		SceneData* sceneData = (SceneData*)OPCMAN.LoadGet("Train.opscene");

		for (ui32 i = 0; i < sceneData->modelsCount; i++) {
			SceneDataModel model = sceneData->models[i];
			LoadSceneModel(model, Transform());
		}
	}


	{
		// // Train Car
		// SceneData* sceneData = (SceneData*)OPCMAN.LoadGet("TrainCar.opscene");

		// for(ui32 j = 0; j < 2; j++) {
		// 	for (ui32 i = 0; i < sceneData->modelsCount; i++) {
		// 		SceneDataModel model = sceneData->models[i];
		// 		LoadSceneModel(model, Transform());
		// 	}
		// }
	}

	// Ground
	{
		for(ui32 i = 0; i < GROUND_COUNT; i++) {
			OPrendererEntity* re = SCENE.Add(CUBE_MODEL, OPrendererEntityDesc(false, true, true, false));
			{
				re->material->Destroy();
				OPfree(re->material);
				re->material = SOLID_MATERIAL->CreateInstances(CUBE_MODEL, false);
			}

			// entityx::Entity e = EX.entities.create();

			//OPrendererEntity* renderEntity = re;
			grounds[i] = re;
			//e.assign<RenderComponent>(renderEntity);
			OPvec3 pos = OPvec3Create(-100.0f + (50.0f * i), -0.5f, 0);
			ground_pos[i] = pos;
			grounds[i]->world.SetTranslate(pos)->Scl(50, 0.1, 100);
			//e.assign<TransformComponent>(pos, OPvec3(50, 0.1, 100), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-1), OPvec3(1)), OPvec3(0, -1, 0));
		}
	}

	// SetWalls(-5 - (trainCarCount * 9.5f), 1.0f);


	LEFT_PHYSX_PLANE->setGlobalPose(PxTransform(PxVec3(-5 - ((TRAINCAR_COUNT - 1) * 9.5f))));
}

void GameState::SpawnEnemy() {
	//chr_man_fix
	//OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet("Monster.opm"), OPrendererEntityDesc(false, true, true, false));



	OPskeleton* skeleton = (OPskeleton*)OPCMAN.LoadGet("MonsterAnim.opm.skel");
	OPrendererEntity* renderEntity = SCENE.Add((OPmodel*)OPCMAN.LoadGet("MonsterAnim.opm"), skeleton, OPrendererEntityDesc(true, true, true, false));
	

	OPskeletonAnimation* anim = (OPskeletonAnimation*)OPCMAN.LoadGet("MonsterAnim.opm.walk.anim");

	entityx::Entity e = EX.entities.create();

	e.assign<AnimationComponent>(skeleton, anim->Clone());

	//OPrendererEntity* renderEntity = re;

	f32 x = TRAIN_POSITION.x + 20.0f + (OPrandom() * 25.0f);// - (TRAINCAR_COUNT * 10.0f);
	//-15.0f * trainCarCount + 
	OPvec3 pos = OPvec3(x, 0, 10.0 + OPrandom() * 10);
	e.assign<TransformComponent>(pos, OPvec3(1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-1, 0, -1), OPvec3(1, 1, 1)), OPVEC3_ZERO);
	e.assign<DirectionComponent>(OPvec3(0, 0, 0), OPvec3(0, -1.0, 0), 0.18f);
	e.assign<RenderComponent>(renderEntity);
	e.assign<PhysicsComponent>(pos, PHYSX_CONTROLLERMANAGER);
	void* aidata = NULL;
	e.assign<AIComponent>(EnemyUpdate, aidata);
	e.assign<HealthComponent>();
	e.assign<DamageComponent>(1, 1000);
	// e.assign<OffsetComponent>();
}

void GameState::SpawnEnemyHard() {
	//chr_man_fix


	OPskeleton* skeleton = (OPskeleton*)OPCMAN.LoadGet("MonsterHardAnim.opm.skel");
	OPrendererEntity* renderEntity = SCENE.Add((OPmodel*)OPCMAN.LoadGet("MonsterHardAnim.opm"), skeleton, OPrendererEntityDesc(true, true, true, false));
	

	OPskeletonAnimation* anim = (OPskeletonAnimation*)OPCMAN.LoadGet("MonsterHardAnim.opm.walk.anim");

	entityx::Entity e = EX.entities.create();

	e.assign<AnimationComponent>(skeleton, anim->Clone());


	f32 x = TRAIN_POSITION.x + 20.0f + (OPrandom() * 25.0f);// - (TRAINCAR_COUNT * 10.0f);
	//-15.0f * trainCarCount + 
	OPvec3 pos = OPvec3(x, 0, 10.0 + OPrandom() * 10);
	e.assign<TransformComponent>(pos, OPvec3(1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-1, 0, -1), OPvec3(1, 1, 1)), OPVEC3_ZERO);
	e.assign<DirectionComponent>(OPvec3(0, 0, 0), OPvec3(0, -1.0, 0), 0.18f);
	e.assign<RenderComponent>(renderEntity);
	e.assign<PhysicsComponent>(pos, PHYSX_CONTROLLERMANAGER);
	void* aidata = NULL;
	e.assign<AIComponent>(EnemyUpdate, aidata);
	e.assign<HealthComponent>(25);
	e.assign<DamageComponent>(2, 1500);
	// e.assign<OffsetComponent>();
}

void GameState::AddTrainCar() {
	// Train Car
	SceneData* sceneData = (SceneData*)OPCMAN.LoadGet("TrainCar.opscene");
	PurchaseSound();

	for (ui32 i = 0; i < sceneData->modelsCount; i++) {
		SceneDataModel model = sceneData->models[i];
		LoadSceneModel(model, Transform());
	}


	entityx::ComponentHandle<TrainComponent> train;
	entityx::ComponentHandle<DirectionComponent> dir;
	for (entityx::Entity entityOther : EX.entities.entities_with_components(train, dir)) {
		dir->maxSpeed = 0.3 - (TRAINCAR_COUNT * 0.025f);
	}
}

bool TimeSlow = false;
OPint GameState::Update(OPtimer* timer) {

	if(SHOW_MENU) {
		if (OPKEYBOARD.WasPressed(OPkeyboardKey::W) || OPGAMEPADS[0]->LeftThumbNowUp()) {
			MENU_SELECTION--;
		}
		if (OPKEYBOARD.WasPressed(OPkeyboardKey::S) || OPGAMEPADS[0]->LeftThumbNowDown()) {
			MENU_SELECTION++;
		}

		if(Arriving || TRAIN_ACTIVATED) {
			MENU_SELECTION = MENU_SELECTION % 2;

			if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) || OPKEYBOARD.WasPressed(OPkeyboardKey::E) || OPGAMEPADS[0]->WasPressed(OPgamePadButton::X)) {
				if(MENU_SELECTION == 0) {
					SHOW_MENU = !SHOW_MENU;
				}
				if(MENU_SELECTION == 1) {
					return true;
				}
			}
		} else {
			MENU_SELECTION = MENU_SELECTION % 4;

			if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) || OPKEYBOARD.WasPressed(OPkeyboardKey::E) || OPGAMEPADS[0]->WasPressed(OPgamePadButton::X)) {
				if(MENU_SELECTION == 2) {
					SHOW_MENU = !SHOW_MENU;
				}

				if( MENU_SELECTION == 1 && MONEY >= 100) {

					if(TRAINCAR_COUNT < 5) {
							MONEY -= 100;
							PurchaseSound();
							AddTrainCar();
							SHOW_MENU = !SHOW_MENU;
					} else {
						CancelSound();
					}
				} else if(MENU_SELECTION == 1) {
					CancelSound();
				}

				if(MENU_SELECTION == 0) {
					SHOW_MENU = !SHOW_MENU;
					entityx::ComponentHandle<TrainComponent> train;
					for (entityx::Entity entityOther : EX.entities.entities_with_components(train)) {
						train->activated = !train->activated;
						TRAIN_ACTIVATED = true;
						DepartSound();
					}
				}

				if(MENU_SELECTION == 3) {
					return true;
				}
			}
		}
		
		if (OPKEYBOARD.WasPressed(OPkeyboardKey::M)) {
			SHOW_MENU = !SHOW_MENU;
		}
	} else if(SHOW_TURRET_MENU) {
		if (OPKEYBOARD.WasPressed(OPkeyboardKey::W) || OPGAMEPADS[0]->LeftThumbNowUp()) {
			MENU_SELECTION--;
		}
		if (OPKEYBOARD.WasPressed(OPkeyboardKey::S) || OPGAMEPADS[0]->LeftThumbNowDown()) {
			MENU_SELECTION++;
		}
		MENU_SELECTION = MENU_SELECTION % 2;

		if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) || OPKEYBOARD.WasPressed(OPkeyboardKey::E) || OPGAMEPADS[0]->WasPressed(OPgamePadButton::X)) {
			if(MENU_SELECTION == 1) {
				SHOW_TURRET_MENU = !SHOW_TURRET_MENU;

			}
			if(MENU_SELECTION == 0) {
				
				// Create turret / check money


				if(MONEY >= 50) {
					MONEY -= 50;
					SHOW_TURRET_MENU = !SHOW_TURRET_MENU;
					PurchaseSound();


					entityx::Entity entity = EX.entities.get(TURRET_ID);

					// Add Laser Turret
					entityx::ComponentHandle<TransformComponent> transformComponent = entity.component<TransformComponent>();
					entityx::ComponentHandle<WobbleComponent> wobbleComponent = entity.component<WobbleComponent>();
					entityx::ComponentHandle<ActivationComponent> activationComponent = entity.component<ActivationComponent>();
					

					OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet("LaserTurret.opm"), OPrendererEntityDesc(false, true, true, false));
					entityx::Entity e = EX.entities.create();
					OPrendererEntity* renderEntity = re;

					OPvec3 pos = transformComponent->startingPos;
					e.assign<TransformComponent>(pos, OPvec3(1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-0.5f), OPvec3(0.5f)), OPVEC3_ZERO);
					e.assign<RenderComponent>(renderEntity);
					e.assign<OffsetComponent>();

					OPrendererEntity* reLaser = SCENE.Add((OPmodel*)OPCMAN.LoadGet("Laser.opm"), OPrendererEntityDesc(false, true, true, false));
					reLaser->alive = false;
					e.assign<WeaponComponent>(reLaser);
					if(entity.has_component<WobbleComponent>()) {
						e.assign<WobbleComponent>(wobbleComponent->index);
					}
					e.assign<DamageComponent>();
				} else {
					CancelSound();
				}
			}
		}
		
		// if (OPKEYBOARD.WasPressed(OPkeyboardKey::M)) {
		// 	SHOW_TURRET_MENU = !SHOW_TURRET_MENU;
		// }
	} else {

		if (OPKEYBOARD.WasPressed(OPkeyboardKey::P)) {
			SpawnEnemy();
		}
		if (OPKEYBOARD.WasPressed(OPkeyboardKey::O)) {
			CAMSHAKE = 150;
		}
		// if (OPKEYBOARD.WasPressed(OPkeyboardKey::M)) {
		// 	SHOW_MENU = !SHOW_MENU;
		// }


		// if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) {
		// 	TimeSlow = !TimeSlow;
		// }

		// if (TimeSlow) {
		// 	if (OPKEYBOARD.IsDown(OPkeyboardKey::SPACE)) {
		// 		timer->Elapsed /= 5.0f;
		// 	}
		// 	else {
		// 		timer->Elapsed = 0;
		// 	}
		// }

		time += timer->Elapsed / 1000.0f;
		GlobalsUpdate(timer);

		if(TRAIN_ACTIVATED && current_wave < wave_count) {
			time_until_next_wave -= timer->Elapsed;
			show_wave -= timer->Elapsed;

			if(time_until_next_wave <= 0) {
				current_wave++;
				if(current_wave >= wave_count) {
					OPlogErr("Winner!");
					TrainStation->world.SetTranslate(OPvec3(TRAIN_POSITION.x + 50, 0, 0));
					// deactivate train
					entityx::ComponentHandle<TrainComponent> train;
					for (entityx::Entity entityOther : EX.entities.entities_with_components(train)) {
						train->activated = false;
					}

					Arriving = true;
					trips++;
					TRAIN_ACTIVATED = false;

				} else {
					time_until_next_wave += waves_time[current_wave];
					show_wave = 1000;
					for(ui32 i = 0; i < waves[current_wave]; i++) {
						SpawnEnemy();
					}
					for(ui32 i = 0; i < waves_hard[current_wave]; i++) {
						SpawnEnemyHard();
					}
				}
			}
		}

		EX.systems.update<InputSystem>(timer->Elapsed);
		EX.systems.update<MovementSystem>(timer->Elapsed);
		EX.systems.update<WeaponSystem>(timer->Elapsed);
		EX.systems.update<PhysicsSystem>(timer->Elapsed);
		// EX.systems.update<ActivationSystem>(timer->Elapsed);
		// EX.systems.update<AttackSystem>(timer->Elapsed);
		// EX.systems.update<AnimationSystem>(timer->Elapsed);

		SCENE.Update(timer);

		if(PLAYERISDEAD) {
			return false;
		}

		for(ui32 i = 0; i < TRACK_COUNT; i++) {
		
			f32 halfWay = TRACK_COUNT;

			if(track_pos[i].x < TRAIN_POSITION.x - TRACK_COUNT) {
				track_pos[i].x += TRACK_COUNT * 2;
				tracks[i]->world.SetTranslate(track_pos[i]);
			}
		}


		for(ui32 i = 0; i < ROCK_COUNT; i++) {
		
			f32 area = 120.0f;
			f32 halfWay = area / 2.0f;

			if(rock_pos[i].x < TRAIN_POSITION.x - halfWay) {
				rock_pos[i].x += area;
				rocks[i]->world.SetTranslate(rock_pos[i])->RotY(OPrandom() * OPpi);
			}
		}


		for(ui32 i = 0; i < GROUND_COUNT; i++) {
			if(ground_pos[i].x < TRAIN_POSITION.x - 100) {
				ground_pos[i].x += (GROUND_COUNT - 1) * 50;
				grounds[i]->world.SetTranslate(ground_pos[i])->Scl(50, 0.1, 100);
			}
		}

		if(Arriving) {

			if(trips == total_trips) {
				return false;
			}

			entityx::ComponentHandle<TrainComponent> train;
			entityx::ComponentHandle<DirectionComponent> dir;
			for (entityx::Entity entityOther : EX.entities.entities_with_components(train, dir)) {
				if(OPabs(dir->velocity.x) < 0.05) {
					Arriving = false;
					current_wave = -1;
					MONEY += (80 * trips);
					PurchaseSound();
					for(ui32 i = 0; i < wave_count; i++) {
						waves[i]++;
					}
					wave_count++;
					break;
				}
			}
		}
		
	}

	return false;
}

void GameState::Render(OPfloat delta) {

	EX.systems.update<RenderSystem>(delta);

	if(CAMSHAKE > 0) {
		f32 percent = (150 - CAMSHAKE ) / 150.0f;
		OPrenderClear(0.3 + percent, 0.3 + percent, 0.6 + percent, 1);
	} else {
		OPrenderClear(0.3, 0.3, 0.6, 1);
	}

	OPvec3 camPos = CAMERA.pos;
	OPvec3 camTarget = CAMERA.target;

	if(CAMSHAKE > 0) {
		f32 percent = (OPpi2 * (150 - CAMSHAKE ) / 150.0f) * 3.0f;
		CAMERA.pos.x += OPsin(percent) * 0.25f;;
		CAMERA.pos.y += OPsin(percent) * 0.25f;
		CAMERA.target.x += OPsin(percent) * 0.25f;
		CAMERA.target.y += OPsin(percent) * 0.25f;
	}
	CAMERA.Update();

	SCENE.Render(delta);
	CAMERA.pos = camPos;
	CAMERA.target = camTarget;

	// OPRENDERER_ACTIVE->SetCull(false);
	// OPRENDERER_ACTIVE->SetCullMode(OPcullFace::FRONT);
	// OPRENDERER_ACTIVE->SetDepthWrite(false);

	// for (ui32 i = 0; i < SPRITE3DSCOUNT; i++) {
	// 	SPRITE3DS[i].Render(SCENE.camera);
	// }

	// // EX.entities.each<ParticleEmitterComponent>([delta](entityx::Entity entity, ParticleEmitterComponent& emitter) {
	// // 	emitter.particleSystem.Render(SCENE.camera);
	// // });

	// OPRENDERER_ACTIVE->SetDepthWrite(true);
	// OPRENDERER_ACTIVE->SetCullMode(OPcullFace::BACK);
	// OPRENDERER_ACTIVE->SetCull(true);



	if(SHOW_MENU) {
		OPrenderCull(false);
		OPtexture2DRender(screenTex);
		FontManager->SetAlign(OPfontAlign::LEFT);
		OPfontRenderBegin(FontManager);
		
		OPfontRender("<", OPvec2(150, 100 + (MENU_SELECTION * 50)));

		if(Arriving || TRAIN_ACTIVATED) {
			OPfontRender("Cancel", OPvec2(180, 100));
			OPfontRender("Exit Game", OPvec2(180, 150));
		} else {
			OPfontRender("Depart", OPvec2(180, 100));
			if(TRAINCAR_COUNT > 4) {
				OPfontRender("Train Cars at Max", OPvec2(180, 150));	
			} else {
				OPfontRender("Purchase Train Car $100", OPvec2(180, 150));	
			}
			OPfontRender("Cancel", OPvec2(180, 200));
			OPfontRender("Exit Game", OPvec2(180, 250));
		}	
		OPfontRenderEnd();

	} else if(SHOW_TURRET_MENU) {
		OPrenderCull(false);
		OPtexture2DRender(screenTex);
		FontManager->SetAlign(OPfontAlign::LEFT);
		OPfontRenderBegin(FontManager);
		OPfontRender("<", OPvec2(150, 100 + (MENU_SELECTION * 50)));
		OPfontRender("Purchase Laser $50", OPvec2(180, 100));	
		OPfontRender("Cancel", OPvec2(180, 150));
		OPfontRenderEnd();
	} else {

		OPfontRenderBegin(FontManager);


		i8 buffer[128];
		if(PLAYERISDEAD) {
			FontManager->SetAlign(OPfontAlign::CENTER);
			OPfontRender("You Dead.", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2.0f, (f32)OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2.0f - 10.0f));	
		} else {
			FontManager->SetAlign(OPfontAlign::LEFT);
			sprintf(buffer, "$%d", MONEY);
			OPfontRender(buffer, OPvec2(10, 50));	
		}

		if(show_wave > 0) {	
			FontManager->SetAlign(OPfontAlign::CENTER);
			sprintf(buffer, "Wave %d", (current_wave + 1));
			OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2.0f, (f32)OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2.0f - 10.0f));	
		}

		entityx::ComponentHandle<InputComponent> input;
		entityx::ComponentHandle<HealthComponent> health;

		for (entityx::Entity entityOther : EX.entities.entities_with_components(input, health)) {
			FontManager->SetAlign(OPfontAlign::LEFT);
			for(ui32 i = 0 ; i < health->health; i++) {
				OPfontRender(">3", OPvec2(10 + i * 40, 10));	
			}
		}

		FontManager->SetAlign(OPfontAlign::CENTER);
		entityx::ComponentHandle<TransformComponent> transform, transform2;
		entityx::ComponentHandle<ActivationComponent> activation;
		for (entityx::Entity entity : EX.entities.entities_with_components(input, transform)) {
			for (entityx::Entity entity2 : EX.entities.entities_with_components(transform2, activation)) {
				
				OPfloat maxDist = 1.0;
				OPfloat dist = OPvec3Dist(transform->pos, transform2->pos);
				if (dist < maxDist) {

					OPvec2 p = CAMERA.Unproject(transform2->pos + TRAIN_POSITION) - OPvec2(0, 150);
					OPfontRender(activation->name, p);	
				}
			}
		}
		
		if(Arriving & !PLAYERISDEAD) {
			if(current_wave >= total_trips) {
				OPfontRender("You won! You saved the day!", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Width / 2.0f, (f32)OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height / 2.0f - 10.0f));	
			}
		}
		

		OPfontRenderEnd();
	}


	OPrenderPresent();
}

void GameState::LoadSceneModel(SceneDataModel model, Transform transform) {

	if (!OPstringEquals(model.type, "GROUP")) {

		if (OPstringEquals(model.gametype, "Train")) {
			TRAINCAR_COUNT++;
			
			OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet(model.opm), OPrendererEntityDesc(false, true, true, false));
			entityx::Entity e = EX.entities.create();
			OPrendererEntity* renderEntity = re;

			e.assign<TransformComponent>(OPVEC3_ZERO, OPvec3(1,1,1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-4.25f, 0, -1.5f), OPvec3(4.25f, 5.0, 1.5f)), OPVEC3_ZERO);
			e.assign<DirectionComponent>(OPvec3(0, 0, 0), OPvec3(0, -1.0, 0), 0.22);
			e.assign<RenderComponent>(renderEntity);
			e.assign<PhysicsComponent>(OPVEC3_ZERO, PHYSX_CONTROLLERMANAGER);
			//e.assign<InputComponent>(1);
			e.assign<TrainComponent>();
		} else if (OPstringEquals(model.gametype, "TrainCar")) {

			TRAINCAR_COUNT++;
			
			OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet(model.opm), OPrendererEntityDesc(false, true, true, false));
			entityx::Entity e = EX.entities.create();
			OPrendererEntity* renderEntity = re;

			e.assign<TransformComponent>(OPvec3(-9.5f * (TRAINCAR_COUNT - 1) + TRAIN_POSITION.x, 1.5f, 0), OPvec3(1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-4.25f, 0, -1.5f), OPvec3(4.25f, 5.0, 1.5f)), OPVEC3_ZERO);
			e.assign<RenderComponent>(renderEntity);
			e.assign<TrainCarComponent>();
			e.assign<WobbleComponent>(TRAINCAR_COUNT);
			e.assign<PhysicsComponent>(OPvec3(-9.5f * (TRAINCAR_COUNT - 1) + TRAIN_POSITION.x, 1.5f, 0), PHYSX_CONTROLLERMANAGER);
			e.assign<DirectionComponent>(OPvec3(0, 0, 0), OPvec3(0, -1.0, 0));
			// e.assign<OffsetComponent>();

			LEFT_PHYSX_PLANE->setGlobalPose(PxTransform(PxVec3(-5 - ((TRAINCAR_COUNT - 1) * 9.5f))));

		} else if (OPstringEquals(model.gametype, "WeaponPlate")) {
			
			OPrendererEntity* re = SCENE.Add((OPmodel*)OPCMAN.LoadGet(model.opm), OPrendererEntityDesc(false, true, true, false));
			entityx::Entity e = EX.entities.create();
			OPrendererEntity* renderEntity = re;

			OPvec3 pos = model.position + OPvec3(-9.5f * (TRAINCAR_COUNT - 1) + TRAIN_POSITION.x, 0, 0) - TRAIN_POSITION;
			e.assign<TransformComponent>(pos, OPvec3(1), OPVEC3_ZERO, OPboundingBox3D(OPvec3(-0.5f), OPvec3(0.5f)), OPVEC3_ZERO);
			e.assign<RenderComponent>(renderEntity);
			e.assign<OffsetComponent>();
			//if(TRAINCAR_COUNT != 0) {
				e.assign<WobbleComponent>(TRAINCAR_COUNT);
			//}
			e.assign<ActivationComponent>(WeaponPlateActivate, nullptr, "Turret");

		} else if (OPstringEquals(model.gametype, "TrainTrigger")) {

			OPrendererEntity* re = SCENE.Add(CUBE_MODEL, OPrendererEntityDesc(false, true, true, false));
			{
				re->material->Destroy();
				OPfree(re->material);
				re->material = SOLID_MATERIAL->CreateInstances(CUBE_MODEL, false);
			}

			entityx::Entity e = EX.entities.create();

			OPrendererEntity* renderEntity = re;

			e.assign<TransformComponent>(model.position - TRAIN_POSITION, model.scale, model.rotation, OPboundingBox3D(OPvec3(-1), OPvec3(1)), OPVEC3_ZERO);
			e.assign<RenderComponent>(renderEntity);
			e.assign<ActivationComponent>(TrainActivate, nullptr, "Console");
			e.assign<OffsetComponent>();
		}

	}

	for (ui32 i = 0; i < model.childrenCount; i++) {
		LoadSceneModel(model.children[i], Transform(transform.translate + model.position, transform.rotate + model.rotation, transform.scale * model.scale));
	}
}

OPint GameState::Exit(OPgameState* next) {

	GlobalsShutdown();
	SCENE.Destroy();
	
	return 0;
}

GameState GS_GAME_STATE;