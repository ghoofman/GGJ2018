#include "InputSystem.h"
#include "../Globals.h"

#include "../Components/TransformComponent.h"
#include "../Components/ActivationComponent.h"

void InputSystem::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {


	entityx::ComponentHandle<TransformComponent> transform, transform2;
	entityx::ComponentHandle<InputComponent> input;
	entityx::ComponentHandle<ActivationComponent> activation;
	for (entityx::Entity entity : es.entities_with_components(input, transform)) {
        
		if (input->controls.PowerUpWasPressed()) {

            for (entityx::Entity entity2 : es.entities_with_components(transform2, activation)) {
                OPfloat maxDist = 1.0;
                OPfloat dist = OPvec3Dist(transform->pos, transform2->pos);
                if (dist < maxDist) {
			        activation->activateFn(&entity2, activation->data);
                }
            }

		}
	}


	es.each<InputComponent, DirectionComponent>([dt](entityx::Entity entity, InputComponent &input, DirectionComponent &direction) {

		f32 drag = 0.5f;
		OPfloat vertical = -0.15;
		OPfloat accel = 0.9;

		OPfloat speed = 1.0f + input.controls.Running();
		direction.speedVariant = speed;

		OPvec3 dir = OPVEC3_ZERO;

		// OPvec3 forward = CAMERA.pos - CAMERA.target;
		// forward.y = 0;
		// forward = OPvec3Norm(forward);

		// OPvec3 left = OPvec3Cross(forward, OPVEC3_UP);

		OPvec2 moveDir = input.controls.Movement();

		if (input.canMove) {
			dir.x = moveDir.x;
			dir.z = 0;
			//dir.z = moveDir.y;
			dir.Norm();
		}

		OPvec2 planerVelocity = {
			direction.velocity.x,
			0//direction.velocity.z
		};

		OPvec2 velDragDir = OPvec2Norm(planerVelocity) * -drag;

		if (direction.velocity.x > 0) {
			direction.velocity.x += velDragDir.x * dt;
			if (direction.velocity.x < 0) direction.velocity.x = 0;
		}
		else if (direction.velocity.x < 0) {
			direction.velocity.x += velDragDir.x * dt;
			if (direction.velocity.x > 0) direction.velocity.x = 0;
		}

		if (direction.velocity.z > 0) {
			direction.velocity.z += velDragDir.y * dt;
			if (direction.velocity.z < 0) direction.velocity.z = 0;
		}
		else if (direction.velocity.z < 0) {
			direction.velocity.z += velDragDir.y * dt;
			if (direction.velocity.z > 0) direction.velocity.z = 0;
		}

		direction.acceleration *= drag;
		direction.acceleration.y = vertical;

		OPfloat len = OPvec3Len(dir);
		if (len > 0) {

			// OPvec3 acceleration;
			//direction.acceleration += forward * (-dir.z * accel * speed);
			direction.acceleration.x -= -dir.x * accel * speed;

			direction.acceleration.y = 0;
		}

	});


	es.each<DirectionComponent>([dt](entityx::Entity entity, DirectionComponent &direction) {

		f32 speed = direction.speedVariant;

		OPfloat maxSpeed = direction.maxSpeed;
		OPfloat vertical = -0.15;

		OPvec3 forward = CAMERA.pos - CAMERA.target;
		forward.y = 0;
		forward = OPvec3Norm(forward);

		OPvec3 left = OPvec3Cross(forward, OPVEC3_UP);

		direction.velocity += direction.acceleration * dt;
		direction.velocity.y = vertical;

		OPvec2 planerVelocity;
		planerVelocity.x = direction.velocity.x;
		planerVelocity.y = direction.velocity.z;
		if (OPvec2Len(planerVelocity) > maxSpeed * speed) {
			planerVelocity = OPvec2Norm(planerVelocity) * maxSpeed * speed;
			direction.velocity.x = planerVelocity.x;
			direction.velocity.z = planerVelocity.y;
		}

		OPvec2 facingDir = OPvec2(direction.acceleration.x, direction.acceleration.z);
		if (OPvec2Len(facingDir) > 0.01) {
			facingDir = OPvec2Norm(facingDir);
			direction.facing = OPvec3(facingDir.x, 0, facingDir.y);
		}
	});


}