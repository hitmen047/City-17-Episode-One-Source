//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef EPISODIC_SCREENSPACEEFFECTS_H
#define EPISODIC_SCREENSPACEEFFECTS_H
#ifdef _WIN32
#pragma once
#endif

#include "screenspaceeffects.h"

class CStunEffect : public IScreenSpaceEffect
{
public:
	CStunEffect( void ) : 
		m_flDuration( 0.0f ), 
		m_flFinishTime( 0.0f ), 
		m_bUpdateView( true ) {}

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params );
	virtual void Enable( bool bEnable ) { m_bEnabled = bEnable; }
	virtual bool IsEnabled( ) { return m_bEnabled; }

	virtual void Render( int x, int y, int w, int h );

private:
	bool		m_bEnabled;

	CTextureReference m_StunTexture;
	CMaterialReference m_EffectMaterial;
	float		m_flDuration;
	float		m_flFinishTime;
	bool		m_bUpdateView;
};

//
//  EP1 Intro Blur
//

class CEP1IntroEffect : public IScreenSpaceEffect
{
public:
	CEP1IntroEffect( void ) : 
		  m_flDuration( 0.0f ), 
		  m_flFinishTime( 0.0f ), 
		  m_bUpdateView( true ),
		  m_bEnabled( false ),
		  m_bFadeOut( false ) {}

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params );
	virtual void Enable( bool bEnable ) { m_bEnabled = bEnable; }
	virtual bool IsEnabled( ) { return m_bEnabled; }

	virtual void Render( int x, int y, int w, int h );

private:
	
	inline unsigned char	GetFadeAlpha( void );

	CTextureReference m_StunTexture;
	CMaterialReference m_EffectMaterial;
	float		m_flDuration;
	float		m_flFinishTime;
	bool		m_bUpdateView;
	bool		m_bEnabled;
	bool		m_bFadeOut;
};

//
//  EP2 Player Stunned Effect
//

//
//  EP1 Intro Blur
//

class CEP2StunEffect : public IScreenSpaceEffect
{
public:
	CEP2StunEffect( void ) : 
	  m_flDuration( 0.0f ), 
	  m_flFinishTime( 0.0f ), 
	  m_bUpdateView( true ),
	  m_bEnabled( false ),
	  m_bFadeOut( false ) {}

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params );
	virtual void Enable( bool bEnable ) { m_bEnabled = bEnable; }
	virtual bool IsEnabled( ) { return m_bEnabled; }

	virtual void Render( int x, int y, int w, int h );

private:

	inline unsigned char	GetFadeAlpha( void );

	CTextureReference m_StunTexture;
	CMaterialReference m_EffectMaterial;
	float		m_flDuration;
	float		m_flFinishTime;
	bool		m_bUpdateView;
	bool		m_bEnabled;
	bool		m_bFadeOut;
};

#endif // EPISODIC_SCREENSPACEEFFECTS_H
