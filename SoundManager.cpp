#include "SoundManager.h"
#include "Player.h"
#include "Globals.h"

SoundManager::SoundManager(void)
	: mEngine(0),
	mLastVolume(-1.0)
{
}


SoundManager::~SoundManager(void)
{
}

vec3df SoundManager::toIrrklang(Vector3 vector)
{
	return vec3df(vector.x, vector.y, vector.z);
}

void SoundManager::update(Player* player)
{
	reloadSoundConfig();
	mEngine->setListenerPosition(toIrrklang(player->getLocation()), toIrrklang(player->getOrientationForAudioLib()));
}

void SoundManager::playNewSound3D(SoundEffects effect, Vector3 position)
{
	if (mSoundSources[effect] != 0)
	{
		mEngine->play3D(mSoundSources[effect], toIrrklang(position));
	}
}

void SoundManager::playNewSound2D(SoundEffects effect)
{
	if (mSoundSources[effect] != 0)
	{
		mEngine->play2D(mSoundSources[effect]);
	}
}

void SoundManager::playNewSound2D(Music music)
{
	if (mMusicSource[music] != 0)
	{
		//mEngine->play2D(mMusicSource[music], true);
	}
}

bool SoundManager::init()
{
	mEngine = createIrrKlangDevice(); 
	mEngine->setDefault3DSoundMinDistance(SOUND_MIN_DISTANCE);
	mEngine->setDefault3DSoundMaxDistance(SOUND_MAX_DISTANCE);
	//mEngine->setDefault3DSoundMaxDistance(SOUND_MAX_DISTANCE);
	//mSoundSources[0]->setDefaultMaxDistance

	memset(mSoundSources, 0, sizeof(mSoundSources));
	memset(mMusicSource, 0, sizeof(mMusicSource));
	
	mSoundSources[SoundEffects::GUI_CLICK] = mEngine->addSoundSourceFromFile("resources/sounds/effects/gui-click.wav");
	mSoundSources[SoundEffects::ON_GRAB_WALL] = mEngine->addSoundSourceFromFile("resources/sounds/effects/wall-grab.wav");
	mSoundSources[SoundEffects::STEP_MOSAIC_1] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-ceramic-1.ogg");
	mSoundSources[SoundEffects::STEP_MOSAIC_2] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-ceramic-2.ogg");
	mSoundSources[SoundEffects::STEP_MOSAIC_3] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-ceramic-3.ogg");
	mSoundSources[SoundEffects::STEP_MOSAIC_4] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-ceramic-4.ogg");
	mSoundSources[SoundEffects::STEP_GRASS_1] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-grass-1.ogg");
	mSoundSources[SoundEffects::STEP_GRASS_2] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-grass-2.ogg");
	mSoundSources[SoundEffects::STEP_GRASS_3] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-grass-3.ogg");
	mSoundSources[SoundEffects::STEP_GRASS_4] = mEngine->addSoundSourceFromFile("resources/sounds/effects/step-grass-4.ogg");
	mSoundSources[SoundEffects::JUMP_NORMAL_GRASS] = mEngine->addSoundSourceFromFile("resources/sounds/effects/jump-normal-grass.ogg");
	mSoundSources[SoundEffects::JUMP_LONG_GRASS] = mEngine->addSoundSourceFromFile("resources/sounds/effects/jump-long-grass.ogg");
	mSoundSources[SoundEffects::WALL_JUMP_WOOD] = mEngine->addSoundSourceFromFile("resources/sounds/effects/wall-jump-wood.ogg");
	
	mMusicSource[Music::PACHELBEL_CANON_IN_D] = mEngine->addSoundSourceFromFile("resources/sounds/music/pachelbel-canon-in-d.ogg");

	reloadSoundConfig();

	return true;
}

void SoundManager::reloadSoundConfig()
{
	if (_::options.muteSoundEffects && mLastVolume != 0.0)
	{
		setSoundConfig(false, 0.0);
	}
	if (!_::options.muteSoundEffects && mLastVolume != _::options.soundEffectsLevel)
	{
		setSoundConfig(!_::options.muteSoundEffects, _::options.soundEffectsLevel);
	}
}

void SoundManager::setSoundConfig(bool enable, double value)
{
	if (enable)
	{
		mLastVolume = value;
		mEngine->setSoundVolume(value);
	}
	else
	{
		mLastVolume = 0.0;
		mEngine->setSoundVolume(0.0);
	}
}

SoundManager* SoundManager::singleton = new SoundManager();