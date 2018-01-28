#include "MovementSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/FollowComponent.h"
#include "../Components/RenderComponent.h"
#include "../Components/OffsetComponent.h"
#include "../Components/TrainComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Globals.h"

void MovementSystem::updateFollowComponents(entityx::EntityManager &es) {

	OPvec3 center = OPVEC3_ZERO;
	f32 count = 0;
	OPfloat maxDist = 0;

	entityx::ComponentHandle<TransformComponent> transform, transform2;
	entityx::ComponentHandle<FollowComponent> follow, follow2;
	for (entityx::Entity entity : es.entities_with_components(transform, follow)) {
		center += transform->pos;
		count++;

		for (entityx::Entity entity2 : es.entities_with_components(transform2, follow2)) {
			OPfloat dist = OPvec3Dist(transform->pos, transform2->pos);
			if (maxDist < dist) {
				maxDist = dist;
			}
		}
	}

	OPfloat moveSpeed = 0.08;

	if (count > 0) {
		center /= count;

		// Center on follow
		//OPvec3 followDist = OPvec3(0, 9, 15);
		OPvec3 followDist = OPvec3(-3, 3, 25);
		if (maxDist > 10) {
			maxDist -= 10;
			followDist *= 1.0 + (maxDist / 15.0f);
		}
		OPvec3 currentLook = CAMERA.target - TRAIN_POSITION;
		OPvec3 targetLook = center;
		OPvec3 diffLook = targetLook - currentLook;

		OPfloat lengthOfLook = OPvec3Len(diffLook);
		//if (lengthOfLook > moveSpeed) {
			diffLook = OPvec3Norm(diffLook);
			diffLook *= moveSpeed * lengthOfLook;
		//}

		OPvec3 currentPos = CAMERA.pos - TRAIN_POSITION;
		OPvec3 targetPos = center + followDist;
		OPvec3 diff = targetPos - currentPos;

		OPfloat lengthOfPos = OPvec3Len(diff);
		//if (lengthOfPos > moveSpeed) {
			diff = OPvec3Norm(diff);
			diff *= moveSpeed * lengthOfPos;
		//}

		CAMERA.SetPerspective(currentPos + diff + TRAIN_POSITION, currentLook + diffLook + TRAIN_POSITION);

        OPfloat shadowCameraSize = 40.0f;
	    SCENE.shadowCamera->SetOrtho(OPvec3(60, 20, 90) + TRAIN_POSITION, OPvec3(0) + TRAIN_POSITION, OPVEC3_UP, 15.0f, 200.0f, -shadowCameraSize, shadowCameraSize, -shadowCameraSize, shadowCameraSize);

	}


}

void MovementSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {

	updateFollowComponents(es);


	entityx::ComponentHandle<TrainComponent> train;
	entityx::ComponentHandle<DirectionComponent> dir;
	for (entityx::Entity entity : es.entities_with_components(train, dir)) {
        if(train->activated) {
		    f32 drag = 0.0002f;
            dir->acceleration.x += 0.01f * dt;
            dir->acceleration *= drag;
        } else {
		    f32 drag = 0.00003f - (TRAINCAR_COUNT * 0.0000025);


            OPvec2 planerVelocity = {
                dir->velocity.x,
                dir->velocity.z
            };

            OPvec2 velDragDir = OPvec2Norm(planerVelocity) * -drag;

            if (dir->velocity.x > 0) {
                dir->velocity.x += velDragDir.x * dt;
                if (dir->velocity.x < 0) dir->velocity.x = 0;
            }
            else if (dir->velocity.x < 0) {
                dir->velocity.x += velDragDir.x * dt;
                if (dir->velocity.x > 0) dir->velocity.x = 0;
            }

            dir->acceleration *= drag;

        }
    }


	es.each<AnimationComponent>([dt](entityx::Entity entity, AnimationComponent &anim) {

		anim.anim->Update(dt, 1.0);

		anim.anim->Apply(anim.skeleton);

		anim.skeleton->Update();

	});


};