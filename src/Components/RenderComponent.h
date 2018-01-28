#pragma once

#include "./Human/include/Rendering/OPrendererEntity.h"

struct RenderComponent {
	RenderComponent(OPrendererEntity* rendererEntity) : rendererEntity(rendererEntity) {}

	OPrendererEntity* rendererEntity;
};