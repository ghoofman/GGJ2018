#pragma once

#include "entityx.h"
#include "../Components/InputComponent.h"
#include "../Components/DirectionComponent.h"

struct InputSystem : public entityx::System<InputSystem> {
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};