//========= Copyright � 1996-2007, Valve Corporation, All rights reserved. ====
//
// An entity that allows level designer control over the fog parameters.
//
//=============================================================================

#include "cbase.h"
#include "fogcontroller.h"
#include "entityinput.h"
#include "entityoutput.h"
#include "eventqueue.h"
#include "player.h"
#include "world.h"
#include "ndebugoverlay.h"
#include "KeyValues.h"
#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CFogSystem s_FogSystem( "FogSystem" );

ConVar debug_fog_lerping("debug_fog_lerping", "0", FCVAR_CHEAT );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CFogSystem *FogSystem( void )
{
	return &s_FogSystem;
}

LINK_ENTITY_TO_CLASS( env_fog_controller, CFogController );

BEGIN_DATADESC( CFogController )

	DEFINE_INPUTFUNC( FIELD_FLOAT,		"SetStartDist",	InputSetStartDist ),
	DEFINE_INPUTFUNC( FIELD_FLOAT,		"SetEndDist",	InputSetEndDist ),
	DEFINE_INPUTFUNC( FIELD_FLOAT,		"SetMaxDensity",	InputSetMaxDensity ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"TurnOn",		InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"TurnOff",		InputTurnOff ),
	DEFINE_INPUTFUNC( FIELD_COLOR32,	"SetColor",		InputSetColor ),
	DEFINE_INPUTFUNC( FIELD_COLOR32,	"SetColorSecondary",	InputSetColorSecondary ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"SetFarZ",		InputSetFarZ ),
	DEFINE_INPUTFUNC( FIELD_STRING,		"SetAngles",	InputSetAngles ),

	DEFINE_INPUTFUNC( FIELD_COLOR32,	"SetColorLerpTo",		InputSetColorLerpTo ),
	DEFINE_INPUTFUNC( FIELD_COLOR32,	"SetColorSecondaryLerpTo",	InputSetColorSecondaryLerpTo ),
	DEFINE_INPUTFUNC( FIELD_FLOAT,		"SetStartDistLerpTo",	InputSetStartDistLerpTo ),
	DEFINE_INPUTFUNC( FIELD_FLOAT,		"SetEndDistLerpTo",	InputSetEndDistLerpTo ),
	DEFINE_INPUTFUNC( FIELD_FLOAT,		"SetMaxDensLerpTo",	InputSetMaxDensLerpTo ),
	DEFINE_INPUTFUNC( FIELD_FLOAT,		"SetFarZLerpTo", InputSetFarZLerpTo ),
	//DEFINE_INPUTFUNC( FIELD_VOID,		"StartFogTransition", InputStartFogTransition ),
	DEFINE_INPUTFUNC( FIELD_BOOLEAN,	"SetUseBlending", InputSetUseBlending ),

	// Quiet classcheck
	//DEFINE_EMBEDDED( m_fog ),

	DEFINE_KEYFIELD( m_iszFogSet,			FIELD_STRING,	"FogScript" ),
	DEFINE_KEYFIELD( m_bUseAngles,			FIELD_BOOLEAN,	"use_angles" ),
	DEFINE_KEYFIELD( m_fog.colorPrimary,	FIELD_COLOR32,	"fogcolor" ),
	DEFINE_KEYFIELD( m_fog.colorSecondary,	FIELD_COLOR32,	"fogcolor2" ),
	DEFINE_KEYFIELD( m_fog.dirPrimary,		FIELD_VECTOR,	"fogdir" ),
	DEFINE_KEYFIELD( m_fog.enable,			FIELD_BOOLEAN,	"fogenable" ),
	DEFINE_KEYFIELD( m_fog.blend,			FIELD_BOOLEAN,	"fogblend" ),
	DEFINE_KEYFIELD( m_fog.start,			FIELD_FLOAT,	"fogstart" ),
	DEFINE_KEYFIELD( m_fog.end,				FIELD_FLOAT,	"fogend" ),
	DEFINE_KEYFIELD( m_fog.maxdensity,		FIELD_FLOAT,	"fogmaxdensity" ),
	DEFINE_KEYFIELD( m_fog.farz,			FIELD_FLOAT,	"farz" ),

	DEFINE_THINKFUNC( SetLerpValues ),

	DEFINE_FIELD( m_iChangedVariables, FIELD_INTEGER ),

	DEFINE_KEYFIELD( m_fog.colorPrimaryLerpTo, FIELD_COLOR32, "fogcolorlerpto" ),
	DEFINE_KEYFIELD( m_fog.colorSecondaryLerpTo, FIELD_COLOR32, "fogcolor2lerpto" ),
	DEFINE_KEYFIELD( m_fog.startLerpTo, FIELD_FLOAT, "fogstartlerpto" ),
	DEFINE_KEYFIELD( m_fog.endLerpTo, FIELD_FLOAT, "fogendlerpto" ),
	DEFINE_KEYFIELD( m_fog.maxdensityLerpTo, FIELD_FLOAT, "fogmaxdensitylerpto" ),
	DEFINE_KEYFIELD( m_fog.farzLerpTo, FIELD_FLOAT, "fogfarzlerpto" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST_NOBASE( CFogController, DT_FogController )
// fog data
	SendPropInt( SENDINFO_STRUCTELEM( m_fog.enable ), 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO_STRUCTELEM( m_fog.blend ), 1, SPROP_UNSIGNED ),
	SendPropVector( SENDINFO_STRUCTELEM(m_fog.dirPrimary), -1, SPROP_COORD),
	SendPropInt( SENDINFO_STRUCTELEM( m_fog.colorPrimary ), 32, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO_STRUCTELEM( m_fog.colorSecondary ), 32, SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO_STRUCTELEM( m_fog.start ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO_STRUCTELEM( m_fog.end ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO_STRUCTELEM( m_fog.maxdensity ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO_STRUCTELEM( m_fog.farz ), 0, SPROP_NOSCALE ),

	SendPropInt( SENDINFO_STRUCTELEM( m_fog.colorPrimaryLerpTo ), 32, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO_STRUCTELEM( m_fog.colorSecondaryLerpTo ), 32, SPROP_UNSIGNED ),
	SendPropFloat( SENDINFO_STRUCTELEM( m_fog.startLerpTo ), 0, SPROP_NOSCALE ),
	SendPropFloat( SENDINFO_STRUCTELEM( m_fog.endLerpTo ), 0, SPROP_NOSCALE ),
END_SEND_TABLE()

#define FOG_FILE "scripts/fog_parameters.txt"

CFogController::CFogController()
{
	// Make sure that old maps without fog fields don't get wacked out fog values.
	m_fog.enable = false;
	m_fog.maxdensity = 1.0f;
	m_iszFogSet = NULL_STRING;

	m_fog.colorPrimaryLerpTo.GetForModify() = m_fog.colorPrimary.Get();
	m_fog.colorSecondaryLerpTo.GetForModify() = m_fog.colorSecondary.Get();

	m_fog.startLerpTo.GetForModify() = m_fog.start.Get();
	m_fog.endLerpTo.GetForModify() = m_fog.end.Get();
	m_fog.maxdensityLerpTo = m_fog.maxdensity.Get();
	m_fog.farzLerpTo = m_fog.farz.Get();
	m_flLerpSpeed = 0.01;
}

CFogController::~CFogController()
{
}

void CFogController::ReadParams( KeyValues *pKeyValue )
{
	if ( pKeyValue == NULL )
	{
		Assert( !"Fog controller couldn't be initialized!" );
		return;
	}

	int r,g,b,a;
	Color Savecolor = pKeyValue->GetColor( "primarycolor" );

	Savecolor.GetColor(r, g, b, a);
	m_fog.colorPrimary.GetForModify().r = (float)r;
	m_fog.colorPrimary.GetForModify().g = (float)g;
	m_fog.colorPrimary.GetForModify().b = (float)b;
	m_fog.colorPrimaryLerpTo.GetForModify() = m_fog.colorPrimary.Get();

	Savecolor = pKeyValue->GetColor( "secondarycolor" );

	Savecolor.GetColor(r, g, b, a);
	m_fog.colorSecondary.GetForModify().r = (float)r;
	m_fog.colorSecondary.GetForModify().g = (float)g;
	m_fog.colorSecondary.GetForModify().b = (float)b;
	m_fog.colorSecondaryLerpTo.GetForModify() = m_fog.colorSecondary.Get();

	m_fog.start.GetForModify() = pKeyValue->GetFloat( "start" );
	m_fog.startLerpTo.GetForModify() = m_fog.start.Get();

	m_fog.end.GetForModify() = pKeyValue->GetFloat( "end" );
	m_fog.endLerpTo.GetForModify() = m_fog.end.Get();

	m_fog.maxdensity.GetForModify() = pKeyValue->GetFloat( "density" );
	m_fog.maxdensityLerpTo = m_fog.maxdensity.Get();

	m_fog.farz.GetForModify() = pKeyValue->GetFloat( "farz" );
	m_fog.farzLerpTo = m_fog.farz.Get();

	m_flLerpSpeed = pKeyValue->GetFloat( "lerpspeed" );
}

void CFogController::PrepareFogParams( const char *pKeyName )
{
	KeyValues *pKV= new KeyValues( "FogFile" );
	if ( !pKV->LoadFromFile( filesystem, FOG_FILE, "MOD" ) )
	{
		pKV->deleteThis();

		Assert( !"Couldn't find fog paramater file! Fog value load aborted!" );
		return;
	}

	KeyValues *pKVSubkey;
	if ( pKeyName )
	{
		pKVSubkey = pKV->FindKey( pKeyName );
		ReadParams( pKVSubkey );
	}

	pKV->deleteThis();
}

void CFogController::Spawn( void )
{
	BaseClass::Spawn();

	if( m_iszFogSet != NULL_STRING )
	{
		PrepareFogParams( (char *)STRING(m_iszFogSet) );
	}

	m_fog.colorPrimaryLerpTo = m_fog.colorPrimary;
	m_fog.colorSecondaryLerpTo = m_fog.colorSecondary;
}

//-----------------------------------------------------------------------------
// Activate!
//-----------------------------------------------------------------------------
void CFogController::Activate( ) 
{
	BaseClass::Activate();

	if ( m_bUseAngles )
	{
		AngleVectors( GetAbsAngles(), &m_fog.dirPrimary.GetForModify() );
		m_fog.dirPrimary.GetForModify() *= -1.0f; 
	}	    
}

void CFogController::StartTransition( void )
{
	SetThink( &CFogController::SetLerpValues );
	SetNextThink( gpGlobals->curtime + 0.01 );
}

void CFogController::SetLerpValuesTo( Color primary, Color secondary, float start, float end, float density, float farz )
{
	int r,g,b,a;
	primary.GetColor( r, g, b, a );
	m_fog.colorPrimaryLerpTo.GetForModify().r = (float)r;
	m_fog.colorPrimaryLerpTo.GetForModify().g = (float)g;
	m_fog.colorPrimaryLerpTo.GetForModify().b = (float)b;

	secondary.GetColor( r, g, b, a );
	m_fog.colorSecondaryLerpTo.GetForModify().r = (float)r;
	m_fog.colorSecondaryLerpTo.GetForModify().g = (float)g;
	m_fog.colorSecondaryLerpTo.GetForModify().b = (float)b;

	m_fog.startLerpTo.GetForModify() = start;
	m_fog.endLerpTo.GetForModify() = end;
	m_fog.maxdensityLerpTo = density;
	m_fog.farzLerpTo = farz;

	StartTransition();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFogController::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

//------------------------------------------------------------------------------
// Purpose: Input handler for setting the fog start distance.
//------------------------------------------------------------------------------
void CFogController::InputSetStartDist(inputdata_t &inputdata)
{
	// Get the world entity.
	m_fog.start = inputdata.value.Float();
}

//------------------------------------------------------------------------------
// Purpose: Input handler for setting the fog end distance.
//------------------------------------------------------------------------------
void CFogController::InputSetEndDist(inputdata_t &inputdata)
{
	// Get the world entity.
	m_fog.end = inputdata.value.Float();
}

//------------------------------------------------------------------------------
// Input handler for setting the maximum density of the fog. This lets us bring
// the start distance in without the scene fogging too much.
//------------------------------------------------------------------------------
void CFogController::InputSetMaxDensity( inputdata_t &inputdata )
{
	m_fog.maxdensity = inputdata.value.Float();
}

//------------------------------------------------------------------------------
// Purpose: Input handler for turning on the fog.
//------------------------------------------------------------------------------
void CFogController::InputTurnOn(inputdata_t &inputdata)
{
	// Get the world entity.
	m_fog.enable = true;
}

//------------------------------------------------------------------------------
// Purpose: Input handler for turning off the fog.
//------------------------------------------------------------------------------
void CFogController::InputTurnOff(inputdata_t &inputdata)
{
	// Get the world entity.
	m_fog.enable = false;
}

//------------------------------------------------------------------------------
// Purpose: Input handler for setting the primary fog color.
//------------------------------------------------------------------------------
void CFogController::InputSetColor(inputdata_t &inputdata)
{
	// Get the world entity.
	m_fog.colorPrimary = inputdata.value.Color32();
}


//------------------------------------------------------------------------------
// Purpose: Input handler for setting the secondary fog color.
//------------------------------------------------------------------------------
void CFogController::InputSetColorSecondary(inputdata_t &inputdata)
{
	// Get the world entity.
	m_fog.colorSecondary = inputdata.value.Color32();
}

void CFogController::InputSetFarZ(inputdata_t &inputdata)
{
	m_fog.farz = inputdata.value.Int();
}


//------------------------------------------------------------------------------
// Purpose: Sets the angles to use for the secondary fog direction.
//------------------------------------------------------------------------------
void CFogController::InputSetAngles( inputdata_t &inputdata )
{
	const char *pAngles = inputdata.value.String();

	QAngle angles;
	UTIL_StringToVector( angles.Base(), pAngles );

	Vector vTemp;
	AngleVectors( angles, &vTemp );
	SetAbsAngles( angles );

	AngleVectors( GetAbsAngles(), &m_fog.dirPrimary.GetForModify() );
	m_fog.dirPrimary.GetForModify() *= -1.0f;
}


//-----------------------------------------------------------------------------
// Purpose: Draw any debug text overlays
// Output : Current text offset from the top
//-----------------------------------------------------------------------------
int CFogController::DrawDebugTextOverlays(void) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		Q_snprintf(tempstr,sizeof(tempstr),"State: %s",(m_fog.enable)?"On":"Off");
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"Start: %3.0f",m_fog.start);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"End  : %3.0f",m_fog.end);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		color32 color = m_fog.colorPrimary;
		Q_snprintf(tempstr,sizeof(tempstr),"1) Red  : %i",color.r);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"1) Green: %i",color.g);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"1) Blue : %i",color.b);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		color = m_fog.colorSecondary;
		Q_snprintf(tempstr,sizeof(tempstr),"2) Red  : %i",color.r);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"2) Green: %i",color.g);
		EntityText(text_offset,tempstr,0);
		text_offset++;

		Q_snprintf(tempstr,sizeof(tempstr),"2) Blue : %i",color.b);
		EntityText(text_offset,tempstr,0);
		text_offset++;
	}
	return text_offset;
}

#define FOG_CONTROLLER_COLORPRIMARY_LERP 1
#define FOG_CONTROLLER_COLORSECONDARY_LERP 2
#define FOG_CONTROLLER_START_LERP 4
#define FOG_CONTROLLER_END_LERP 8

void CFogController::InputSetColorLerpTo(inputdata_t &data)
{
	m_iChangedVariables |= FOG_CONTROLLER_COLORPRIMARY_LERP;
	m_fog.colorPrimaryLerpTo = data.value.Color32();
}

void CFogController::InputSetColorSecondaryLerpTo(inputdata_t &data)
{
	m_iChangedVariables |= FOG_CONTROLLER_COLORSECONDARY_LERP;
	m_fog.colorSecondaryLerpTo = data.value.Color32();
}

void CFogController::InputSetStartDistLerpTo(inputdata_t &data)
{
	m_iChangedVariables |= FOG_CONTROLLER_START_LERP;
	m_fog.startLerpTo = data.value.Float();
}

void CFogController::InputSetEndDistLerpTo(inputdata_t &data)
{
	m_iChangedVariables |= FOG_CONTROLLER_END_LERP;
	m_fog.endLerpTo = data.value.Float();
}

void CFogController::InputSetMaxDensLerpTo(inputdata_t &data)
{
	m_fog.maxdensityLerpTo = data.value.Float();
}

void CFogController::InputSetFarZLerpTo(inputdata_t &data)
{
	m_fog.farzLerpTo = data.value.Float();
}

/*void CFogController::InputStartFogTransition(inputdata_t &data)
{
	DevMsg( "Fog Lerp starting.\n" );
	StartTransition();
}*/

void CFogController::InputSetUseBlending(inputdata_t &data)
{
	m_fog.blend = data.value.Bool();
}

/*void CFogController::LerpFog( color32 *changefog, color32 *lerpfog )
{
	float flLerpColor[3] = { lerpfog->r, lerpfog->g, lerpfog->b };
	changefog->r = Lerp( 0.01, (float)changefog->r, flLerpColor[1] );
	changefog->g = Lerp( 0.01, (float)changefog->g, flLerpColor[2] );
	changefog->b = Lerp( 0.01, (float)changefog->b, flLerpColor[3] );
}*/

void CFogController::SetLerpValues( void )
{
	color32 colorPrimary = m_fog.colorPrimary;
	color32 colorSecondary = m_fog.colorSecondary;
	color32 colorPrimaryLerpTo = m_fog.colorPrimaryLerpTo;
	color32 colorSecondaryLerpTo = m_fog.colorSecondaryLerpTo;

	if(!(colorPrimary.r == colorPrimaryLerpTo.r))
	{
		colorPrimary.r = Lerp( m_flLerpSpeed, colorPrimary.r, colorPrimaryLerpTo.r );
		m_fog.colorPrimary.GetForModify().r = colorPrimary.r;
	}
	if(!(colorPrimary.g == colorPrimaryLerpTo.g))
	{
		colorPrimary.g = Lerp( m_flLerpSpeed, colorPrimary.g, colorPrimaryLerpTo.g );
		m_fog.colorPrimary.GetForModify().g = colorPrimary.g;
	}
	if(!(colorPrimary.b == colorPrimaryLerpTo.b))
	{
		colorPrimary.b = Lerp( m_flLerpSpeed, colorPrimary.b, colorPrimaryLerpTo.b );
		m_fog.colorPrimary.GetForModify().b = colorPrimary.b;
	}

	if(!(colorSecondary.r == colorSecondaryLerpTo.r))
	{
		colorSecondary.r = Lerp( m_flLerpSpeed, colorSecondary.r, colorSecondaryLerpTo.r );
		m_fog.colorSecondary.GetForModify().r = colorSecondary.r;
	}
	if(!(colorSecondary.g == colorSecondaryLerpTo.g))
	{
		colorSecondary.g = Lerp( m_flLerpSpeed, colorSecondary.g, colorSecondaryLerpTo.g );
		m_fog.colorSecondary.GetForModify().g = colorSecondary.g;
	}
	if(!(colorSecondary.b == colorSecondaryLerpTo.b))
	{
		colorSecondary.b = Lerp( m_flLerpSpeed, colorSecondary.b, colorSecondaryLerpTo.b );
		m_fog.colorSecondary.GetForModify().b = colorSecondary.b;
	}

	if(!(m_fog.start.Get() == m_fog.startLerpTo.Get()))
	{
		m_fog.start.GetForModify() = Lerp( m_flLerpSpeed, m_fog.start.Get(), m_fog.startLerpTo.Get() );
	}
	if(!(m_fog.end.Get() == m_fog.endLerpTo.Get()))
	{
		m_fog.end.GetForModify() = Lerp( m_flLerpSpeed, m_fog.end.Get(), m_fog.endLerpTo.Get() );
	}
	if(!(m_fog.maxdensity.Get() == m_fog.maxdensityLerpTo))
	{
		m_fog.maxdensity.GetForModify() = Lerp( m_flLerpSpeed, m_fog.maxdensity.Get(), m_fog.maxdensityLerpTo );
	}
	if(!(m_fog.farz.Get() == m_fog.farzLerpTo))
	{
		m_fog.farz.GetForModify() = Lerp( m_flLerpSpeed, m_fog.farz.Get(), m_fog.farzLerpTo );
	}

	if( debug_fog_lerping.GetBool() )
	{
		DevMsg( "Color Primary R:%.2f G:%.2f B:%.2f\n", (float)m_fog.colorPrimary.Get().r, (float)m_fog.colorPrimary.Get().g, (float)m_fog.colorPrimary.Get().b );
		DevMsg( "Color Secondary R:%.2f G:%.2f B:%.2f\n", (float)m_fog.colorSecondary.Get().r, (float)m_fog.colorSecondary.Get().g, (float)m_fog.colorSecondary.Get().b );
		DevMsg( "Start: %.2f\n", m_fog.start.Get() );
		DevMsg( "End: %.2f\n", m_fog.end.Get() );
		DevMsg( "Density: %.2f\n", m_fog.maxdensity.Get() );
		DevMsg( "FarZ: %.2f\n", m_fog.farz.Get() );
	}

	SetNextThink( gpGlobals->curtime + 0.01 );
}

//-----------------------------------------------------------------------------
// Purpose: Clear out the fog controller.
//-----------------------------------------------------------------------------
void CFogSystem::LevelInitPreEntity( void )
{
	m_pMasterController = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: On level load find the master fog controller.  If no controller is 
//			set as Master, use the first fog controller found.
//-----------------------------------------------------------------------------
void CFogSystem::LevelInitPostEntity( void )
{
	CFogController *pFogController = NULL;
	do
	{
		pFogController = static_cast<CFogController*>( gEntList.FindEntityByClassname( pFogController, "env_fog_controller" ) );
		if ( pFogController )
		{
			if ( m_pMasterController == NULL )
			{
				m_pMasterController = pFogController;
			}
			else
			{
				if ( pFogController->IsMaster() )
				{
					m_pMasterController = pFogController;
				}
			}
		}
	} while ( pFogController );

	// HACK: Singleplayer games don't get a call to CBasePlayer::Spawn on level transitions.
	// CBasePlayer::Activate is called before this is called so that's too soon to set up the fog controller.
	// We don't have a hook similar to Activate that happens after LevelInitPostEntity
	// is called, or we could just do this in the player itself.
	if ( gpGlobals->maxClients == 1 )
	{
		CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
		if ( pPlayer && ( pPlayer->m_Local.m_PlayerFog.m_hCtrl.Get() == NULL ) )
		{
			pPlayer->InitFogController();
		}
	}
}

