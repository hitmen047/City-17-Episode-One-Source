#ifndef FMOD_MANAGER_H
#define FMOD_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "fmod.hpp"

class CFMODManager
{
public:
	CFMODManager();
	~CFMODManager();
 
	void InitFMOD();
	void ExitFMOD();

	void FadeThink();

	bool IsSoundPlaying( const char* pathToFileFromSoundsFolder );

	void PlayAmbientSound( const char* pathToFileFromSoundsFolder, bool fadeIn );
	void StopAmbientSound( bool fadeOut );
	void TransitionAmbientSounds( const char* pathToFileFromSoundsFolder );

	void PauseAmbientSound( bool bPause );
	bool IsPaused( void );

private:
	void CheckResult( const char* failedSystem );

	const char* GetFullPathToSound( const char* pathToFileFromModFolder );
	const char* GetCurrentSoundName( void );

	const char* currentSound;
	const char* newSoundFileToTransitionTo;
	const char* lastSound;
	bool m_bShouldTransition;
	bool m_bFadeIn;
	bool m_bFadeOut;
	float m_fFadeDelay;
};
 
extern CFMODManager* FMODManager();
 
#include "saverestore.h"
 
ISaveRestoreBlockHandler *GetFMODSaveRestoreBlockHandler();

#endif //FMOD_MANAGER_H