#pragma once

#include "entityx.h"

struct MovementSystem : public entityx::System<MovementSystem> {

	void updateFollowComponents(entityx::EntityManager &es);

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};