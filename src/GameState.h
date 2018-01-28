#pragma once

#include "./OPengine.h"
#include "SceneLoader.h"
#include "Transform.h"

#define TRACK_COUNT 75
#define ROCK_COUNT 15
#define GROUND_COUNT 5


#define MAX_WAVE_COUNT 10

class GameState : public OPgameState {
	OPrendererForward rendererForward;
    
	OPfontManager* FontManager;
	OPfloat time = 0;
    OPrendererEntity* tracks[TRACK_COUNT];
    OPvec3 track_pos[TRACK_COUNT];

    OPrendererEntity* rocks[ROCK_COUNT];
    OPvec3 rock_pos[TRACK_COUNT];

    OPrendererEntity* grounds[GROUND_COUNT];
    OPvec3 ground_pos[GROUND_COUNT];

    ui32 waves[MAX_WAVE_COUNT];
    ui32 waves_hard[MAX_WAVE_COUNT];
    i32 waves_time[MAX_WAVE_COUNT];
    i32 time_until_next_wave;
    i32 current_wave;
    i32 show_wave;
    i32 wave_count = 2;

    i32 total_trips = 5;
    i32 trips = 0;

    OPrendererEntity* TrainStation;

	OPspriteSystem spriteSystem;    
	OPsprite* sprites[2];
	OPeffect spriteEffect;

    OPcam ScreenCamera;

    OPtexture2DOLD* screenTex;
    ui32 selection = 0;
    
    void Init(OPgameState* last);
    OPint Update(OPtimer* timer);
    void Render(OPfloat delta);
    OPint Exit(OPgameState* next);


    void InitScene();
    void LoadSceneModel(SceneDataModel model, Transform transform);
    void SpawnEnemy();
    void SpawnEnemyHard();
    void AddTrainCar();
};

extern GameState GS_GAME_STATE;
