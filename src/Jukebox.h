#pragma once

#include "./OPengine.h"

struct Jukebox {
	OPfmodSound** songs;
	ui32 songCount;
	OPfmodSound* active = NULL;
	OPfmodChannel* activeChannel = NULL;
	i32 current = 0;
	bool started = false;

	void Init();
	void Add(const OPchar* song);
	void Play();
	void Next();
	void Stop();
	void Update();
	void Destroy();
};