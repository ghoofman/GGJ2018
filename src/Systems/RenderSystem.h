#pragma once

#include "entityx.h"

struct RenderSystem : public entityx::System<RenderSystem> {
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};