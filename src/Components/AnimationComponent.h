#pragma once

#include "./OPengine.h"

struct AnimationComponent {
	AnimationComponent(OPskeleton* _skeleton, OPskeletonAnimation* _anim) {
		skeleton = _skeleton;
        anim = _anim;
	}

	OPskeleton* skeleton;
	OPskeletonAnimation* anim;
};