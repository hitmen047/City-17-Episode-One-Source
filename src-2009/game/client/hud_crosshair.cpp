//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hud_crosshair.h"
#include "iclientmode.h"
#include "view.h"
#include "vgui_controls/controls.h"
#include "vgui/ISurface.h"
#include "IVRenderView.h"
#include "facesdk/head_tracking_convars.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar crosshair( "crosshair", "1", FCVAR_ARCHIVE );
ConVar cl_observercrosshair( "cl_observercrosshair", "1", FCVAR_ARCHIVE );

using namespace vgui;

int ScreenTransform( const Vector& point, Vector& screen );

DECLARE_HUDELEMENT( CHudCrosshair );

CHudCrosshair::CHudCrosshair( const char *pElementName ) :
  CHudElement( pElementName ), BaseClass( NULL, "HudCrosshair" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pCrosshair = 0;

	m_clrCrosshair = Color( 0, 0, 0, 0 );

	m_vecCrossHairOffsetAngle.Init();

	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_CROSSHAIR );
}

void CHudCrosshair::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	m_pDefaultCrosshair = gHUD.GetIcon("crosshair_default");
	SetPaintBackgroundEnabled( false );

    SetSize( ScreenWidth(), ScreenHeight() );
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudCrosshair::ShouldDraw( void )
{
	bool bNeedsDraw;

	if ( m_bHideCrosshair )
		return false;

	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return false;

	C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();
	if ( pWeapon && !pWeapon->ShouldDrawCrosshair() )
		return false;

	/* disabled to avoid assuming it's an HL2 player.
	// suppress crosshair in zoom.
	if ( pPlayer->m_HL2Local.m_bZooming )
		return false;
	*/

	// draw a crosshair only if alive or spectating in eye
	if ( IsX360() )
	{
		bNeedsDraw = m_pCrosshair && 
			!engine->IsDrawingLoadingImage() &&
			!engine->IsPaused() && 
			( !pPlayer->IsSuitEquipped() || g_pGameRules->IsMultiplayer() ) &&
			g_pClientMode->ShouldDrawCrosshair() &&
			!( pPlayer->GetFlags() & FL_FROZEN ) &&
			( pPlayer->entindex() == render->GetViewEntity() ) &&
			( pPlayer->IsAlive() ||	( pPlayer->GetObserverMode() == OBS_MODE_IN_EYE ) || ( cl_observercrosshair.GetBool() && pPlayer->GetObserverMode() == OBS_MODE_ROAMING ) );
	}
	else
	{
		bNeedsDraw = m_pCrosshair && 
			crosshair.GetInt() &&
			!engine->IsDrawingLoadingImage() &&
			!engine->IsPaused() && 
			g_pClientMode->ShouldDrawCrosshair() &&
			!( pPlayer->GetFlags() & FL_FROZEN ) &&
			( pPlayer->entindex() == render->GetViewEntity() ) &&
			!pPlayer->IsInVGuiInputMode() &&
			( pPlayer->IsAlive() ||	( pPlayer->GetObserverMode() == OBS_MODE_IN_EYE ) || ( cl_observercrosshair.GetBool() && pPlayer->GetObserverMode() == OBS_MODE_ROAMING ) );
	}

	return ( bNeedsDraw && CHudElement::ShouldDraw() );
}

void CHudCrosshair::Paint( void )
{
	if ( !m_pCrosshair )
		return;

	if ( !IsCurrentViewAccessAllowed() )
		return; 

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if( faceapi_mode.GetInt() > 0 )
	{
		if ( !pPlayer )
			return;

		Vector vecStart, vecStop, vecDirection, vecCrossPos;
		trace_t tr;
			
		AngleVectors(pPlayer->EyeAngles(), &vecDirection);

		vecStart= pPlayer->EyePosition();
		vecStop = vecStart + vecDirection * MAX_TRACE_LENGTH;
			
		UTIL_TraceLine( vecStart, vecStop, MASK_ALL, pPlayer , COLLISION_GROUP_NONE, &tr );
			
			
		ScreenTransform(tr.endpos, vecCrossPos);

		m_pCrosshair->DrawSelf( 0.5f*ScreenWidth()+0.5f*ScreenWidth()*vecCrossPos[0]-0.5f*m_pCrosshair->Width(), 
					0.5f*ScreenHeight()+(0.5f*ScreenHeight()*-vecCrossPos[1])-0.5f*m_pCrosshair->Height(),
					m_clrCrosshair );
	}
	else
	{
		m_curViewAngles = CurrentViewAngles();
		m_curViewOrigin = CurrentViewOrigin();

		float x, y;
		x = ScreenWidth()/2;
		y = ScreenHeight()/2;

		// MattB - m_vecCrossHairOffsetAngle is the autoaim angle.
		// if we're not using autoaim, just draw in the middle of the 
		// screen
		if ( m_vecCrossHairOffsetAngle != vec3_angle )
		{
			QAngle angles;
			Vector forward;
			Vector point, screen;

			// this code is wrong
			angles = m_curViewAngles + m_vecCrossHairOffsetAngle;
			AngleVectors( angles, &forward );
			VectorAdd( m_curViewOrigin, forward, point );
			ScreenTransform( point, screen );

			x += 0.5f * screen[0] * ScreenWidth() + 0.5f;
			y += 0.5f * screen[1] * ScreenHeight() + 0.5f;
		}

		m_pCrosshair->DrawSelf( 
				x - 0.5f * m_pCrosshair->Width(), 
				y - 0.5f * m_pCrosshair->Height(),
				m_clrCrosshair );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCrosshair::SetCrosshairAngle( const QAngle& angle )
{
	VectorCopy( angle, m_vecCrossHairOffsetAngle );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCrosshair::SetCrosshair( CHudTexture *texture, Color& clr )
{
	m_pCrosshair = texture;
	m_clrCrosshair = clr;
}

//-----------------------------------------------------------------------------
// Purpose: Resets the crosshair back to the default
//-----------------------------------------------------------------------------
void CHudCrosshair::ResetCrosshair()
{
	SetCrosshair( m_pDefaultCrosshair, Color(255, 255, 255, 255) );
}
