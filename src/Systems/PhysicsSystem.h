#pragma once

#include "entityx.h"

struct PhysicsSystem : public entityx::System<PhysicsSystem> {
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};
