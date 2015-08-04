#pragma once
#include "stdafx.h"
#include "balance.h"

using namespace irrklang;

class Player;

class SoundManager
{
public:
	enum SoundEffects
	{
		GUI_CLICK,
		STEP_GRASS_1,
		STEP_GRASS_2,
		STEP_GRASS_3,
		STEP_GRASS_4,
		STEP_MOSAIC_1,
		STEP_MOSAIC_2,
		STEP_MOSAIC_3,
		STEP_MOSAIC_4,
		WALL_JUMP_ROCK,
		WALL_JUMP_WOOD,
		JUMP_NORMAL_GRASS,
		JUMP_LONG_GRASS,
		ON_GRAB_WALL,
		SOUND_EFFECTS_SIZE,
	};
	enum Music
	{
		PACHELBEL_CANON_IN_D,
		MUSIC_SIZE
	};
	static SoundManager* singleton;
	void playNewSound3D(SoundEffects, Vector3);
	void playNewSound2D(SoundEffects);
	void playNewSound2D(Music);
	bool init();
	void update(Player*);
	
	ISoundSource* mMusicSource[Music::MUSIC_SIZE];
	ISoundSource* mSoundSources[SoundEffects::SOUND_EFFECTS_SIZE];
	static vec3df toIrrklang(Vector3);
	void reloadSoundConfig();
	void setSoundConfig(bool enable, double value);
private:
	ISoundEngine* mEngine;
	double mLastVolume;
	SoundManager(void);
	~SoundManager(void);
};