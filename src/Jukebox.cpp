#include "Jukebox.h"

void Jukebox::Init() {
	songs = OPALLOC(OPfmodSound*, 32);
	current = 0;
}

void Jukebox::Add(const OPchar* song) {
	OPfmodSound* sound = OPfmodLoad(song);
	songs[songCount++] = sound;
}

void Jukebox::Play() {
	started = true;
	current = 0;
	active = (OPfmodSound*)songs[current];
	activeChannel = OPfmodPlay(active);
}

void Jukebox::Next() {
	current++;
	if (current >= songCount) {
		current = 0;
	}
	active = songs[current];
	activeChannel = OPfmodPlay(active);
}

void Jukebox::Stop() {
	started = false;
}

void Jukebox::Update() {
	if (!started || active == NULL) return;

	OPfmodUpdate();
	if (!OPfmodIsPlaying(activeChannel)) {
		active = NULL;
		activeChannel = NULL;
		Next();
	}
}

void Jukebox::Destroy() {
	OPfree(songs);
}