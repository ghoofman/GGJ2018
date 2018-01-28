#include "PhysicsSystem.h"


#include "../Components/PhysicsComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/RenderComponent.h"
#include "../Components/AIComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/PhysicsStaticComponent.h"
#include "../Components/PhysicsDynamicComponent.h"
#include "../Components/OffsetComponent.h"
#include "../Components/TrainComponent.h"
#include "../Components/TrainCarComponent.h"

#include "../GameState.h"

void PhysicsSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {

	entityx::ComponentHandle<TransformComponent> transform;
	entityx::ComponentHandle<TransformComponent> transform2;
	entityx::ComponentHandle<PhysicsDynamicComponent> physDyn;
	

	entityx::ComponentHandle<TrainComponent> train;
	entityx::ComponentHandle<TrainCarComponent> trainCar;
	entityx::ComponentHandle<DirectionComponent> dirTrain;
	entityx::ComponentHandle<DirectionComponent> dirTrainCar;
	for (entityx::Entity entity : es.entities_with_components(transform, train, dirTrain)) {
        for (entityx::Entity entityCar : es.entities_with_components(transform2, trainCar, dirTrainCar)) {
            //transform->pos.x = TRAIN_POSITION.x;
            dirTrainCar->acceleration = dirTrain->acceleration;
            dirTrainCar->velocity = dirTrain->velocity;
            dirTrainCar->facing = dirTrain->facing;
        }

    }

	es.each<PhysicsComponent, TransformComponent, DirectionComponent>([dt](entityx::Entity entity, PhysicsComponent &phyics, TransformComponent &transform, DirectionComponent &direction) {
		transform.q = OPquat::CreateRot(OPvec3(0, 1, 0), OPpi_2 + OPatan2(direction.facing.x, -direction.facing.z));

		OPphysXControllerMove(phyics.controller, direction.velocity + direction.additionalOffset, dt);
		//OPlogErr("Velocity Add, %f, %f, %f", direction.additionalOffset.x, direction.additionalOffset.y, direction.additionalOffset.z);
		direction.additionalOffset = OPVEC3_ZERO;
	});


	// es.each<ActivateComponent>([dt](entityx::Entity entity, ActivateComponent &activated) {
	// 	f32 delta = dt;
	// 	if (activated.currentlyActive && activated.updateActiveFn != NULL) {
	// 		activated.updateActiveFn(&entity, &delta);
	// 	}
	// 	else if (!activated.currentlyActive && activated.updateInactive != NULL) {
	// 		activated.updateInactive(&entity, &delta);
	// 	}

	// });

	es.each<PhysicsComponent, TransformComponent, DirectionComponent>([dt](entityx::Entity entity, PhysicsComponent &phyics, TransformComponent &transform, DirectionComponent &direction) {
		transform.pos = OPphysXControllerGetFootPos(phyics.controller);
	});
    



	// EX.entities.each<ParticleEmitterComponent, TransformComponent, ActivateComponent>([dt](entityx::Entity entity, ParticleEmitterComponent& emitter, TransformComponent& transform, ActivateComponent& activate) {
	// 	emitter.particleSystem.Update(dt);

	// 	if (!activate.currentlyActive) return;

	// 	emitter.elapsed += dt;
	// 	ui32 spawnFrequency = 0;
	// 	if (emitter.elapsed > spawnFrequency) {
	// 		OPparticle* p = emitter.particleSystem.Spawn(emitter.sprite);
	// 		p->Position = transform.pos;
	// 		p->Position.y += (transform.bounds.max.y - transform.bounds.min.y) / 2.0;
	// 		p->MaxLife = 750 + OPrandom() * 250;
	// 		p->Scale = 0.25 + OPrandom() * 0.25;
	// 		p->ScaleChange = -0.0001;
	// 		OPfloat spread = 0.001;
	// 		OPfloat speed = 0.0025;
	// 		p->Velocity = OPvec3(-speed, -(spread / 2.0) + OPrandom() * spread, -(spread / 2.0) + OPrandom() * spread);
	// 		p->AngularVelo = -0.02 + OPrandom() * 0.04;
	// 		p->Angle = OPrandom();
	// 		emitter.elapsed -= spawnFrequency;
	// 	}
		
	// });

	es.each<AIComponent>([dt](entityx::Entity entity, AIComponent &ai) {
		ai.Update(&entity, dt);
	});

    // make sure each AI is still correctly positioned
	es.each<PhysicsComponent, TransformComponent, AIComponent>([dt](entityx::Entity entity, PhysicsComponent &phyics, TransformComponent &transform, AIComponent &direction) {
		transform.pos = OPphysXControllerGetFootPos(phyics.controller);
	});



	es.each<HealthComponent, TransformComponent>([dt](entityx::Entity entity, HealthComponent &health, TransformComponent& transform) {
        if(!entity.has_component<OffsetComponent>()) {
            if(transform.pos.x < TRAIN_POSITION.x - 50.0f) {
                health.health = 0;
            }
        }
        if(health.health <= 0) {
            // kill entity
            OPlogErr("Killing Entity");

            if(entity.has_component<InputComponent>()) {
                PLAYERISDEAD = true;
            }

            if (entity.has_component<PhysicsComponent>()) {
                entityx::ComponentHandle<PhysicsComponent> p = entity.component<PhysicsComponent>();
                p->controller->release();
            }

            // TODO: (garrett) This should remove it from the scene entirely... not just hide it
            if (entity.has_component<RenderComponent>()) {
                entityx::ComponentHandle<RenderComponent> r = entity.component<RenderComponent>();
                if (r->rendererEntity->alive) {
                    SCENE.Remove(r->rendererEntity);
                }
                entity.remove<RenderComponent>();
            }

            entity.destroy();
        }
	});

    OPphysXSceneUpdate(PHYSX_SCENE, 16);
    OPphysXSceneUpdate(PHYSX_TRAIN_SCENE, 16);


	for (entityx::Entity entity : es.entities_with_components(transform, train, dirTrain)) {
        TRAIN_POSITION.x = transform->pos.x;
        TRAIN_SPEED = dirTrain->velocity.x;
    }


}