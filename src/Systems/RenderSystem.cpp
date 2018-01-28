#include "RenderSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/RenderComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/PhysicsDynamicComponent.h"
#include "../Components/PhysicsStaticComponent.h"
#include "../Components/OffsetComponent.h"
#include "../Components/WeaponComponent.h"
#include "../Components/WobbleComponent.h"
#include "../Components/TrainComponent.h"

void RenderSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {

	// Get Dynamic matrices
	es.each<PhysicsDynamicComponent, TransformComponent, RenderComponent>([dt](entityx::Entity entity, PhysicsDynamicComponent& dyn, TransformComponent &transform, RenderComponent &renderComponent) {
		PxQuat q = dyn.physx->getGlobalPose().q;
		transform.q = OPquat(q.x, q.y, q.z, q.w);

		PxVec3 p = dyn.physx->getGlobalPose().p;
		transform.pos = OPvec3(p.x, p.y, p.z);
	});

	entityx::ComponentHandle<TrainComponent> train;
	entityx::ComponentHandle<DirectionComponent> dirTrain;
	entityx::ComponentHandle<TransformComponent> transform;
	for (entityx::Entity entity : es.entities_with_components(transform, train, dirTrain)) {
        TRAIN_POSITION.x = transform->pos.x;
    }
    
	// Set renderer entities lerped world state
	es.each<TransformComponent, RenderComponent>([dt](entityx::Entity entity, TransformComponent &transform, RenderComponent &renderComponent) {
		renderComponent.rendererEntity->world.
			SetTranslate(
				OPvec3Tween(
					transform.prevPos,
					transform.pos,
					dt))->
			Scl(OPvec3Tween(
				transform.prevScl,
				transform.scl,
				dt))->
			Rot(transform.prevQ.Lerp(transform.q, dt) * OPquat::CreateRot(OPvec3(0, 1, 0), transform.baseRotation.y));
	});

	// Set renderer entities lerped world state
	es.each<TransformComponent, RenderComponent, OffsetComponent>([dt](entityx::Entity entity, TransformComponent &transform, RenderComponent &renderComponent, OffsetComponent &offsetComponent) {
        
        // OPvec3 add = (transform.bounds.max - transform.bounds.min);
        // add.x *= -0.5;
        // add.y *= -0.5;
        // add.z *= -0.5;
        OPvec3 add = OPvec3Tween(PREV_TRAIN_POSITION, TRAIN_POSITION, dt);

        renderComponent.rendererEntity->world.
			SetTranslate(
				OPvec3Tween(
					transform.prevPos + add,
					transform.pos + add,
					dt))->
			Scl(OPvec3Tween(
				transform.prevScl,
				transform.scl,
				dt))->
			Rot(transform.prevQ.Lerp(transform.q, dt) * OPquat::CreateRot(OPvec3(0, 1, 0), transform.baseRotation.y));
	});


	// Set renderer entities lerped world state
	es.each<TransformComponent, RenderComponent, WobbleComponent>([dt](entityx::Entity entity, TransformComponent &transform, RenderComponent &renderComponent, WobbleComponent &wobbleComponent) {
        
        // OPvec3 add = (transform.bounds.max - transform.bounds.min);
        // add.x *= -0.5;
        // add.y *= -0.5;
        // add.z *= -0.5;

        OPvec3 add = OPVEC3_ZERO;

        if(entity.has_component<OffsetComponent>()) {
            add = OPvec3Tween(PREV_TRAIN_POSITION, TRAIN_POSITION, dt);
            //add += TRAIN_POSITION;
        }

        renderComponent.rendererEntity->world.
            SetRotX(OPsin(WOBBLES[wobbleComponent.index] / 300.0f) * TRAIN_SPEED * 0.1f)->
			Translate(
				OPvec3Tween(
					transform.prevPos + add,
					transform.pos + add,
					dt))->
			Scl(OPvec3Tween(
				transform.prevScl,
				transform.scl,
				dt))->
			Rot(transform.prevQ.Lerp(transform.q, dt) * OPquat::CreateRot(OPvec3(0, 1, 0), transform.baseRotation.y));


        if(entity.has_component<WeaponComponent>()) {

	        entityx::ComponentHandle<WeaponComponent> weaponComponent = entity.component<WeaponComponent>();
            if(weaponComponent->hasTarget && weaponComponent->projectile != NULL) {
                // get distance to target

                weaponComponent->projectile->world.
                    SetRotX(OPsin(WOBBLES[wobbleComponent.index] / 300.0f) * TRAIN_SPEED * 0.1f)->
                    Translate(
                        OPvec3Tween(
                            transform.prevPos + add,
                            transform.pos + add,
                            dt))->
                    Scl(OPvec3Tween(
                        transform.prevScl * weaponComponent->distanceToTarget,
                        transform.scl * weaponComponent->distanceToTarget,
                        dt))->
                    Rot(transform.prevQ.Lerp(transform.q, dt) * OPquat::CreateRot(OPvec3(0, 1, 0), transform.baseRotation.y));
            }
        }
	});


	// Adjust for rendering
	es.each<TransformComponent>([dt](entityx::Entity entity, TransformComponent &transform) {
		transform.prevPos = transform.pos;
		transform.prevScl = transform.scl;
		transform.prevQ = transform.q;
	});

    PREV_TRAIN_POSITION = TRAIN_POSITION;
}