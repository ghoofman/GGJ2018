#pragma once

#include "entityx.h"
#include "../Components/InputComponent.h"
#include "../Components/DirectionComponent.h"

struct WeaponSystem : public entityx::System<WeaponSystem> {
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};