// Purpose: Haptics handling function
//


#ifndef _XBOX
#include <windows.h>
#endif

#include "utlvector.h"

#include "cbase.h"

#include "in_haptics.h"		// High-level haptics
#include "haptics_main.h"	// Low-level haptics

#include "basehandle.h"
#include "utlvector.h"
#include "cdll_client_int.h"
#include "cdll_util.h"
#include "kbutton.h"
#include "usercmd.h"
//#include "keydefs.h"
#include "inputsystem/ButtonCode.h"
#include "input.h"
#include "iviewrender.h"
#include "convar.h"
#include "hud.h"
#include "vgui/isurface.h"
#include "vgui_controls/controls.h"
#include "vgui/cursor.h"
#include "vgui/iinput.h"
#include "vgui/IInputInternal.h"
#include "vgui/mousecode.h"
//#include "vstdlib/icommandline.h"
#include "tier0/icommandline.h"
#include "inputsystem/iinputsystem.h"
#include "inputsystem/ButtonCode.h"

#include "iclientvehicle.h"

#include "in_buttons.h"
#include "ThresholdInfo.h"

#ifdef _XBOX
#include "xbox/xbox_platform.h"
#include "xbox/xbox_win32stubs.h"
#include "xbox/xbox_core.h"
#else
#include "../common/xbox/xboxstubs.h"
#endif

#ifdef HL2_CLIENT_DLL
// FIXME: Autoaim support needs to be moved from HL2_DLL to the client dll, so this include should be c_baseplayer.h
#include "c_basehlplayer.h"
#endif

#ifdef PSIMOD
#include "psimod/psimod_player_shared.h"
#endif
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// for internal input through vgui
extern vgui::IInputInternal *g_InputInternal;


// Control like a joystick
#define HAP_ABSOLUTE_AXIS	0x00000000		
// Control like a mouse, spinner, trackball
#define HAP_RELATIVE_AXIS	0x00000010		

//ConVars
#if 1 // used only to accordian massive convar section.
// Buttons
static ConVar hap_debug_button("hap_debug_button","0",FCVAR_ARCHIVE);
#define UI_BUTTON_HELP_STRING "Register button to a mouse button.\n  0 = (Hold)Move Cursor\n  1 = LeftMouse\n  2 = RightMouse\n  3 = Mouse3\n  4 = Mouse4\n  5 = Mouse5\n"
// (for buttons while in cursor / vgui mode)
static ConVar hap_cursor_button1("hap_cursor_button1","1",FCVAR_ARCHIVE,UI_BUTTON_HELP_STRING);
static ConVar hap_cursor_button2("hap_cursor_button2","2",FCVAR_ARCHIVE,UI_BUTTON_HELP_STRING);
static ConVar hap_cursor_button3("hap_cursor_button3","3",FCVAR_ARCHIVE,UI_BUTTON_HELP_STRING);
static ConVar hap_cursor_button4("hap_cursor_button4","4",FCVAR_ARCHIVE,UI_BUTTON_HELP_STRING);
// Just put in a static array for cleaner code.
static ConVar *s_cvUIButtons[]=
{
	&hap_cursor_button1,
	&hap_cursor_button2,
	&hap_cursor_button3,
	&hap_cursor_button4,
};

// Cursor ( note: cursor calls are done on vgui_falconpanel.cpp )
ConVar hap_cursor("hap_cursor","1",FCVAR_ARCHIVE,"Enables cursor control via the haptics input device.");
ConVar hap_debug_cursor("hap_debug_cursor","0",FCVAR_ARCHIVE);
ConVar hap_cursor_sensitivity("hap_cursor_sensitivity","2.0",FCVAR_ARCHIVE,"Movement multiplier of UI cursor movement");


// Axis mapping
static ConVar hap_name( "hap_name", "falcon", FCVAR_ARCHIVE );
static ConVar hap_advanced( "hap_advanced", "0", 0 );
static ConVar hap_advaxisx( "hap_advaxisx", "0", 0 );
static ConVar hap_advaxisy( "hap_advaxisy", "0", 0 );
static ConVar hap_advaxisz( "hap_advaxisz", "0", 0 );
static ConVar hap_advaxisr( "hap_advaxisr", "0", 0 );
static ConVar hap_advaxisu( "hap_advaxisu", "0", 0 );
static ConVar hap_advaxisv( "hap_advaxisv", "0", 0 );

extern ConVar lookspring;
extern ConVar cl_forwardspeed;
extern ConVar lookstrafe;
extern ConVar m_pitch;
extern ConVar l_pitchspeed;
extern ConVar cl_sidespeed;
extern ConVar cl_yawspeed;
extern ConVar cl_pitchdown;
extern ConVar cl_pitchup;
extern ConVar cl_pitchspeed;

static CFastTimer gtToggleTimer;
static int gbClearAvatarForces = 0;

// Misc
static ConVar hap_display_input("hap_display_input", "0", FCVAR_ARCHIVE);
ConVar hap_autosprint("hap_autosprint", "0", 0, "Automatically sprint when moving with an analog joystick" );

static ConVar in_haptics("in_haptics", "1", FCVAR_ARCHIVE);
static ConVar in_haptics_effects("in_haptics_effects", "1");

// Basic "dead zone" and sensitivity
static ConVar hap_forwardthreshold( "hap_forwardthreshold", "0.035", FCVAR_ARCHIVE );
static ConVar hap_forwardsensitivity( "hap_forwardsensitivity", "-1", FCVAR_ARCHIVE );

static ConVar hap_sidethreshold( "hap_sidethreshold", "0.035", FCVAR_ARCHIVE );
static ConVar hap_sidesensitivity( "hap_sidesensitivity", "0.035", FCVAR_ARCHIVE );

static ConVar hap_pitchthreshold( "hap_pitchthreshold", "0.03", FCVAR_ARCHIVE );
static ConVar hap_pitchsensitivity_posctrl( "hap_pitchsensitivity_posctrl", "-120.0", FCVAR_ARCHIVE );
static ConVar hap_pitchsensitivity_velctrl( "hap_pitchsensitivity_velctrl", "-1", FCVAR_ARCHIVE );

static ConVar hap_pitch_analog( "hap_pitch_analog", "1", FCVAR_ARCHIVE, "enables scaling of pitch rate" );

static ConVar hap_yawthreshold( "hap_yawthreshold", "0.03", FCVAR_ARCHIVE );
static ConVar hap_yawsensitivity_posctrl( "hap_yawsensitivity_posctrl", "-120", FCVAR_ARCHIVE );
static ConVar hap_yawsensitivity_velctrl( "hap_yawsensitivity_velctrl", "-2", FCVAR_ARCHIVE );

static ConVar hap_yaw_analog( "hap_yaw_analog", "1", FCVAR_ARCHIVE, "enables scaling of yaw rate" ); 

static ConVar hap_view_mode( "hap_view_mode", "0", FCVAR_ARCHIVE, "view mode, 0 = Box, 1 = Circle" );
static ConVar hap_transition_time("hap_transition_time", "0.10", FCVAR_ARCHIVE, "Transition time duration, used mainly for exiting vehicles and menus." );
ConVar hap_view_broken("hap_view_broken","0",FCVAR_ARCHIVE,"Broken view aiming. this is not fully implemented.");
// Advanced sensitivity and response (generally don't need to mess with)
static ConVar hap_adv_response_move( "hap_adv_response_move", "1", FCVAR_ARCHIVE, "'Movement' stick response mode: 0=Linear, 1=quadratic, 2=cubic, 3=quadratic extreme" );
static ConVar hap_adv_response_look( "hap_adv_response_look", "0", FCVAR_ARCHIVE, "DISABLED - 'Look' stick response mode: 0=Linear, 1=quadratic, 2=cubic, 3=quadratic extreme, 4=custom" );
static ConVar hap_adv_lowend( "hap_adv_lowend", "1", FCVAR_ARCHIVE );
static ConVar hap_adv_lowmap( "hap_adv_lowmap", "1", FCVAR_ARCHIVE );
static ConVar hap_adv_accelscale( "hap_adv_accelscale", "0.6", FCVAR_ARCHIVE);
static ConVar hap_adv_autoaimdampenrange( "hap_adv_autoaimdampenrange", "0", FCVAR_ARCHIVE, "The stick range where autoaim dampening is applied. 0 = off" );
static ConVar hap_adv_autoaimdampen( "hap_adv_autoaimdampen", "0", FCVAR_ARCHIVE, "How much to scale user stick input when the gun is pointing at a valid target." );

// Axes variables
static ConVar hap_axis_relative_samples( "hap_axis_relative_samples", "2", 0,
"Number of samples in relative axis queue.  Higher number yields a greater range. Usually doesn't need to be more than 2");
static ConVar hap_debug_axis("hap_debug_axis", "0", FCVAR_ARCHIVE);

// Footstep effect variables
ConVar hap_debug_steps("hap_debug_steps", "0", FCVAR_ARCHIVE);

ConVar hap_step_time_regular( "hap_step_time_regular", "200", FCVAR_ARCHIVE, "Number of haptics cycles this effect should last." );
ConVar hap_step_peakpct_regular( "hap_step_peakpct_regular", "0.10", FCVAR_ARCHIVE, "The percentage (0.0 to 1.0) of the effect duration in which the peak value occurs." );
ConVar hap_step_scale_regular("hap_step_scale_regular", "1.0", FCVAR_ARCHIVE);

ConVar hap_step_time_splash( "hap_step_time_splash", "200", FCVAR_ARCHIVE, "Number of haptics cycles this effect should last." );
ConVar hap_step_peakpct_splash( "hap_step_peakpct_splash", "0.10", FCVAR_ARCHIVE, "The percentage (0.0 to 1.0) of the effect duration in which the peak value occurs." );
ConVar hap_step_scale_splash("hap_step_scale_splash", "1.0", FCVAR_ARCHIVE);

ConVar hap_step_time_wade( "hap_step_time_wade", "200", FCVAR_ARCHIVE, "Number of haptics cycles this effect should last." );
ConVar hap_step_peakpct_wade( "hap_step_peakpct_wade", "0.10", FCVAR_ARCHIVE, "The percentage (0.0 to 1.0) of the effect duration in which the peak value occurs." );
ConVar hap_step_scale_wade("hap_step_scale_wade", "1.0", FCVAR_ARCHIVE);

// Recoil effect variables
ConVar hap_recoil_time( "hap_recoil_time", "50", FCVAR_ARCHIVE,
"Number of haptics cycles this effect should last." );
ConVar hap_recoil_peakpct( "hap_recoil_peakpct", "0.10", FCVAR_ARCHIVE,
"The percentage (0.0 to 1.0) of the effect duration in which the peak value occurs." );
ConVar hap_recoil_scale("hap_recoil_scale", "0.30", FCVAR_ARCHIVE);
ConVar hap_recoil_scale_angle("hap_recoil_scale_angle", "1.0");
ConVar hap_debug_recoil("hap_debug_recoil", "0", FCVAR_ARCHIVE);

// Damage effect variables
ConVar hap_damage_time( "hap_damage_time", "20", FCVAR_ARCHIVE,
"Number of haptics cycles this effect should last." );
ConVar hap_damage_peakpct( "hap_damage_peakpct", "0.05", FCVAR_ARCHIVE,
"The percentage (0.0 to 1.0) of the effect duration in which the peak value occurs." );
ConVar hap_damage_scale("hap_damage_scale", "0.50", FCVAR_ARCHIVE);
ConVar hap_debug_damage("hap_debug_damage", "0", FCVAR_ARCHIVE);

// Weight carry effect variables
ConVar hap_carry_scale("hap_carry_scale", "1.0", FCVAR_ARCHIVE);
ConVar hap_debug_carry("hap_debug_carry", "0", FCVAR_ARCHIVE);



// Vehicle effect variables
ConVar hap_vehicle_force_scale("hap_vehicle_force_scale", "1.0", FCVAR_ARCHIVE, "Scales vehicle forces");

static int bKeyboardTime = 0;

ConVar hap_var_avatar_movement_mode("hap_var_avatar_movement_mode", "2", FCVAR_NONE,
"Mode 1: Throttle = Keyboard, Turn = Device, Look = Locked ; Mode 2: Throttle = Keyboard, Turn = Keyboard, Look = Locked");
ConVar hap_var_vehicle_movement_mode("hap_var_vehicle_movement_mode", "1", FCVAR_NONE,
"Mode 1: Throttle = Keyboard, Turn = Device, Look = Locked ; Mode 2: Throttle = Keyboard, Turn = Keyboard, Look = Locked");
ConVar hap_vehicle_threshold_turn("hap_vehicle_threshold_turn", "0.01", FCVAR_ARCHIVE);
ConVar hap_vehicle_threshold_throttle("hap_vehicle_threshold_throttle", "0.03", FCVAR_ARCHIVE);
ConVar hap_debug_vehicle("hap_debug_vehicle", "0", FCVAR_ARCHIVE);
ConVar hap_vehicle_smooth("hap_vehicle_smooth", "0", FCVAR_REPLICATED);
ConVar hap_vehicle_constant("hap_vehicle_constant", "1");
ConVar hap_vehicle_shape("hap_vehicle_shape", "1.0");

// Avatar effect variables
ConVar hap_avatar_disable("hap_avatar_disable", "0", FCVAR_ARCHIVE);
ConVar hap_debug_avatar("hap_debug_avatar", "0", FCVAR_ARCHIVE);
ConVar hap_avatar_samplerate("hap_avatar_samples","20",FCVAR_ARCHIVE);
ConVar hap_avatar_scale("hap_avatar_scale", "2.0", FCVAR_ARCHIVE);
// Avatar threshold data.
ConVar hap_avatar_scale_x("hap_avatar_scale_x", "-0.01", FCVAR_ARCHIVE);
ConVar hap_avatar_scale_y("hap_avatar_scale_y", "-0.01", FCVAR_ARCHIVE);
ConVar hap_avatar_scale_z("hap_avatar_scale_z", "-0.01", FCVAR_ARCHIVE);
ConVar hap_avatar_force_threshold_lower("hap_avatar_force_threshold_lower", "5.0", FCVAR_ARCHIVE);
ConVar hap_avatar_force_threshold_upper("hap_avatar_force_threshold_upper", "165", FCVAR_ARCHIVE);
ConVar hap_avatar_scale_low("hap_avatar_scale_low", "1.0", FCVAR_ARCHIVE);
ConVar hap_avatar_scale_medium("hap_avatar_scale_medium", "1.0", FCVAR_ARCHIVE);
ConVar hap_avatar_scale_high("hap_avatar_scale_high", "2.0", FCVAR_ARCHIVE);
// Avatar Threshold Helper 

static const ThresholdInfo s_ThresholdAvatar(	&hap_avatar_force_threshold_lower, &hap_avatar_scale_low,
												&hap_avatar_scale_medium,
												&hap_avatar_force_threshold_upper, &hap_avatar_scale_high,
												&hap_avatar_scale_x,
												&hap_avatar_scale_y,
												&hap_avatar_scale_z);

#ifndef TABLE_ONLY
static ConVar hap_testint("hap_testint", "0", FCVAR_REPLICATED);
#endif

#endif // 1

#define HAPTICS_INIT_ON_STARTUP false 
//#define HAPTICS_INIT_ON_STARTUP (hap_cursor.GetInt()==0) // cvars are loaded after so this doesnt work.

typedef struct 
{
	float	averageValue;
	CUtlVector<float> sampleQueue;
} relative_t;

relative_t	hapRelativeData[2];

static float g_averageYawValue;
static float g_averagePitchValue;
static CUtlVector<float> g_yawData;
static CUtlVector<float> g_pitchData;

int var_hap_togglehaptics = 0;
int var_hap_togglemovement = 0;

extern kbutton_t	in_hap_holdcamera;

int IntToBinary(int num)
{
	int retnum = 0;
	if(num & 1)
		retnum += 1;
	if(num & 2)
		retnum += 10;
	if(num & 4)
		retnum += 100;
	if(num & 8)
		retnum += 1000;

	return retnum;
}

//-----------------------------------------------
// Response curve function for the move axes
//-----------------------------------------------
static float ResponseCurve( int curve, float x )
{
	switch ( curve )
	{
	case 1:
		// quadratic
		if ( x < 0 )
			return -(x*x);
		return x*x;

	case 2:
		// cubic
		return x*x*x;

	case 3:
		{
		// quadratic extreme
		float extreme = 1.0f;
		if ( fabs( x ) >= 0.95f )
		{
			extreme = 1.5f;
		}
		if ( x < 0 )
			return -extreme * x*x;
		return extreme * x*x;
		}
	}

	// linear
	return x;
}


//-----------------------------------------------
// If we have a valid autoaim target, dampen the 
// player's stick input if it is moving away from
// the target.
//
// This assists the player staying on target.
//-----------------------------------------------
float HapAutoAimDampening( float x, int axis, float dist )
{
	// FIXME: Autoaim support needs to be moved from HL2_DLL to the client dll, so all games can use it.
#ifdef HL2_CLIENT_DLL
	// Help the user stay on target if the feature is enabled and the user
	// is not making a gross stick movement.
	if( hap_adv_autoaimdampen.GetFloat() > 0.0f && fabs(x) < hap_adv_autoaimdampenrange.GetFloat() )
	{
		// Get the HL2 player
		C_BaseHLPlayer *pLocalPlayer = (C_BaseHLPlayer *)C_BasePlayer::GetLocalPlayer();

		if( pLocalPlayer )
		{
			// Get the autoaim target.
			CBaseEntity *pTarget = pLocalPlayer->m_HL2Local.m_hAutoAimTarget.Get();

			if( pTarget )
			{
				return hap_adv_autoaimdampen.GetFloat();
			}
		}
	}
#endif
	return 1.0f;// No dampening.
}


//-----------------------------------------------
// This structure holds persistent information used
// to make decisions about how to modulate analog
// stick input.
//-----------------------------------------------
typedef struct 
{
	float	envelopeScale[2];
} envelope_t;

envelope_t	hapControlEnvelope;


//-----------------------------------------------
// Response curve function specifically for the 
// 'look' analog stick.
//
// when AXIS == YAW, otherAxisValue contains the 
// value for the pitch of the control stick, and
// vice-versa.
//-----------------------------------------------
static float ResponseCurveLook( int curve, float x, int axis, float otherAxis, float dist )
{
	float input = x;

	// Make X positive to make things easier, just remember whether we have to flip it back!
	bool negative = false;
	if( x < 0.0f )
	{
		negative = true;
		x *= -1;
	}

	// Perform the two-stage mapping.
	if( x > hap_adv_lowend.GetFloat() )
	{
		float highmap = 1.0f - hap_adv_lowmap.GetFloat();
		float xNormal = x - hap_adv_lowend.GetFloat();

		float factor = xNormal / ( 1.0f - hap_adv_lowend.GetFloat() );
		x = hap_adv_lowmap.GetFloat() + (highmap * factor);

		// Accelerate.
		if( hapControlEnvelope.envelopeScale[axis] < 1.0f )
		{
			float delta = x - hap_adv_lowmap.GetFloat();

			x = hap_adv_lowmap.GetFloat() + (delta * hapControlEnvelope.envelopeScale[axis]);

			hapControlEnvelope.envelopeScale[axis] += ( gpGlobals->frametime * hap_adv_accelscale.GetFloat() );

			if( hapControlEnvelope.envelopeScale[axis] > 1.0f )
			{
				hapControlEnvelope.envelopeScale[axis] = 1.0f;
			}
		}
	}
	else
	{
		// Shut off acceleration
		hapControlEnvelope.envelopeScale[axis] = 0.0f;

		float factor = x / hap_adv_lowend.GetFloat();
		x = hap_adv_lowmap.GetFloat() * factor;
	}

	x *= HapAutoAimDampening( input, axis, dist );

	if( axis == PITCH && input != 0.0f && hap_display_input.GetBool() )
	{
		Msg("In:%f Out:%f\n", input, x );
	}

	if( negative )
	{
		x *= -1;
	}

	return x;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : index - 
// Output : char const
//-----------------------------------------------------------------------------
char const *C_Haptics::HapDescribeAxis( int index )
{
	switch ( index )
	{
	case GAME_AXIS_FORWARD:
		return "Forward";
	case GAME_AXIS_PITCH:
		return "Look";
	case GAME_AXIS_SIDE:
		return "Side";
	case GAME_AXIS_YAW:
		return "Turn";
	case GAME_AXIS_NONE:
	default:
		return "Unknown";
	}

	return "Unknown";
}

//-----------------------------------------------------------------------------
// Purpose: Scales the raw analog value to lie withing the axis range (full range - deadzone )
//-----------------------------------------------------------------------------
float C_Haptics::HapScaleAxisValue( const float axisValue, const float axisThreshold )
{
	// Xbox scales the range of all axes in the inputsystem. PC can't do that because each axis mapping
	// has a (potentially) unique threshold value.  If all axes were restricted to a single threshold
	// as they are on the Xbox, this function could move to inputsystem and be slightly more optimal.
	float result = 0.f;
	// 1.0 replaced MAX_BUTTONSAMPLE
	if ( IsPC() )
	{
		if ( axisValue < -axisThreshold )
		{
			result = ( axisValue + axisThreshold ) / ( 1.0 - axisThreshold );
		}
		else if ( axisValue > axisThreshold )
		{
			result = ( axisValue - axisThreshold ) / ( 1.0 - axisThreshold );
		}
	}
	else
	{
		// IsXbox
		result =  axisValue * ( 1.f / MAX_BUTTONSAMPLE );
	}

	return result;
}

// Haptics --------------------------------------------------------------------
// Purpose: quick function to control internal vgui input based on the above convars.( 0 == button1 .. 3 == button4 )
//-----------------------------------------------------------------------------
static void UIButtonState(int button,bool down)
{
	if(s_cvUIButtons[button]->GetInt()==0)
	{
		// zero is not a mouse click. it is for non constant cursor sampling. if it is held down
		// the cursor will be moved ( considering always on cursor is off )
		return;
	}
	else
	{
		if(down)
			g_InputInternal->InternalMousePressed( vgui::MouseCode(s_cvUIButtons[button]->GetInt()-1) );
		else
			g_InputInternal->InternalMouseReleased( vgui::MouseCode(s_cvUIButtons[button]->GetInt()-1) );
	}
}

// Haptics --------------------------------------------------------------------
// Purpose: Processes buttons from device.
//-----------------------------------------------------------------------------
void C_Haptics::HapButtonEvent( int bstate )
{
	// perform button actions
	for (int i=0 ; i<m_nHapticsButtons ; i++)
	{
		// Only fire changed buttons
		int nBit = 1 << i;
		if( (bstate & nBit) != (m_nHapticsOldButtons & nBit) )
		{
			// we are in game, so send the game buttons.
			m_TempButton = KEY_HAPTIC1;
			const char *binding = engine->Key_BindingForKey( m_TempButton );
			input->KeyEvent( bstate & nBit, KEY_HAPTIC1, binding );
			if(hap_debug_button.GetInt()==1)
			{
				Msg("Button: %i [%s]\n",i+1,(bstate&nBit)?"DOWN":" UP ");
			}
			
			// Important this peice of code is after the key_event. (for binding purposes)
			
			if(InUIMode())// add check to hap_cursor
			{
				// we are in cursor mode. so process
				UIButtonState(i, bstate & nBit );
			}
		}
	}
	m_nHapticsOldButtons = bstate;
}

// Haptics --------------------------------------------------------------------
// Purpose: Checks if a button is down.
//-----------------------------------------------------------------------------
bool C_Haptics::IsButtonDown(int Number)
{
	int nBit = 1 << (Number-1);// subtract one to match the button Numbering.
	return m_nHapticsOldButtons & nBit;
}

// Haptics --------------------------------------------------------------------
// Purpose: Read's the input data from the haptics device.
//-----------------------------------------------------------------------------
bool C_Haptics::ReadHaptics ( void )
{
	if (hdlHaptics::instance())
        hdlHaptics::instance()->syncFromServo();

	double x = 0;
    double y = 0;
    double z = 0;
	double rawx = 0;
	double rawy = 0;
	double rawz = 0;
    int l_buttonState = 0;

	if (hdlHaptics::instance())
	{
		hdlHaptics::instance()->getRelativePosition(x, y, z);
		hdlHaptics::instance()->getAbsolutePosition(rawx, rawy, rawz);
		//this is for listed button lookup. getDeviceSwitchState returns true as long as the list is not empty.
		while(hdlHaptics::instance()->getDeviceSwitchState(l_buttonState))
		{
			// press or release the buttons as applicable
			HapButtonEvent(l_buttonState);
		}
		
		//This is for standard one look up buttons
		//hdlHaptics::instance()->getDeviceSwitchState(l_buttonState);
		//HapButtonEvent(l_buttonState);
	}

	m_flHapticsPosition[HAP_AXIS_X] = (float)x;
	m_flHapticsPosition[HAP_AXIS_Y] = (float)y;
	m_flHapticsPosition[HAP_AXIS_Z] = (float)z;

	m_flRawHapticsPosition[HAP_AXIS_X] = (float)rawx;
	m_flRawHapticsPosition[HAP_AXIS_Y] = (float)rawy;
	m_flRawHapticsPosition[HAP_AXIS_Z] = (float)rawz;

	
	return true; // being optimistic

}

// Haptics --------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
C_Haptics::C_Haptics( void )
{

	//variable initialization.
	m_flAverageDeltaTime = 0;


	m_fHapticsInit = false;

	// used for mouse movement in vgui
	m_bMovingCursor = false;

	// vehicle mode
	m_bUpdateVehicle = false;

	VehicleForceThreshold = (ThresholdInfo *)&s_ThresholdAvatar;

}

// Haptics --------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
C_Haptics::~C_Haptics( void )
{
	//Shutdown_Haptics(); // destructor is too quick for hardware shutdown?
}

// Haptics --------------------------------------------------------------------
// Purpose: Set's internal init notice
//-----------------------------------------------------------------------------
void C_Haptics::Set_Init( bool bstate )
{
	m_fHapticsInit = bstate;
}

// Haptics --------------------------------------------------------------------
// Purpose: Advanced haptics setup
//-----------------------------------------------------------------------------
void C_Haptics::Init_Haptics(void)
{
	//BA CES07
	if(m_fHapticsInit)
		return;
	// called whenever an update is needed
	int	i;

	// Initialize all the maps
	for ( i = 0; i < MAX_HAPTICS_AXES; i++ )
	{
		m_rgHapAxes[i].AxisMap = GAME_AXIS_NONE;
		m_rgHapAxes[i].ControlMap = HAP_ABSOLUTE_AXIS;
	}

	// default haptics initialization
	// 3 axes only with haptics control
	m_rgHapAxes[HAP_AXIS_X].AxisMap = GAME_AXIS_YAW;
	m_rgHapAxes[HAP_AXIS_Y].AxisMap = GAME_AXIS_PITCH;
	m_rgHapAxes[HAP_AXIS_Z].AxisMap = GAME_AXIS_FORWARD;

	// Initialize device
	hdlHaptics::create();
	hdlHaptics *instancePtr = hdlHaptics::instance();
    if (instancePtr)
	{
		instancePtr->start();
	}

	m_nHapticsButtons = 4;
	m_fHapticsInit = true;

	// initialize variables
	m_nHapticsOldButtons = 0;

	Msg("\n- Haptics has been initialized\n");
	Msg("- Device name: %s\n", instancePtr->m_deviceName);
	Msg("- Extents:\n");
	Msg("- x = %+0.6f to %+0.6f\n",
		instancePtr->m_deviceWorkspaceDim[0],
		instancePtr->m_deviceWorkspaceDim[3]);
	Msg("- y = %+0.6f to %+0.6f\n",
		instancePtr->m_deviceWorkspaceDim[1],
		instancePtr->m_deviceWorkspaceDim[4]);
	Msg("- z = %+0.6f to %+0.6f\n\n",
		instancePtr->m_deviceWorkspaceDim[2],
		instancePtr->m_deviceWorkspaceDim[5]);

	m_flTransitionTime = gpGlobals->curtime;

}

// Haptics --------------------------------------------------------------------
// Purpose: Shut down the haptics device.
//-----------------------------------------------------------------------------
void C_Haptics::Shutdown_Haptics (void)
{
    if(!m_fHapticsInit)
		return;
	if (hdlHaptics::instance())
	{
		hdlHaptics::instance()->stop();
		hdlHaptics::instance()->destroy();
	}

	m_fHapticsInit = false;

}
// Haptics --------------------------------------------------------------------
// Purpose: Called on level init. We use it to check our current control mode.
// that way we dont cause problems when being in a vehicle and then moving to 
// a new level.
// m_bUpdateVehicle could really just be pPlayer->IsInVehicle i suppose.
//-----------------------------------------------------------------------------
void C_Haptics::LevelInit()
{
	m_flTransitionTime = gpGlobals->curtime;
	//m_tVelocityTimer.Start();
}

static bool bButtonCleared = false;
ConVar hap_cursor_absolute("hap_cursor_absolute","1",FCVAR_ARCHIVE);
// Haptics --------------------------------------------------------------------
// Purpose: Sets x and y to calculated mouse position. returns whether the mouse should actualy be moving to it or not.
//-----------------------------------------------------------------------------
bool C_Haptics::GetCursorPosition(int *x,int *y)
{
	//set our transition timer to current time. (that way when we are done with using this cursor the position aiming will transition)
	m_flTransitionTime = gpGlobals->curtime;
	if(InUIMode())
	{
		// because we dont do the read haptics pump ushualy when the cursor is visible, we will do it now.
		if ( ReadHaptics() )
		{
			bool MouseMove = ( ( hap_cursor_button1.GetInt()==0 && IsButtonDown(1) ) ||
							   ( hap_cursor_button2.GetInt()==0 && IsButtonDown(2) ) ||
							   ( hap_cursor_button3.GetInt()==0 && IsButtonDown(3) ) ||
							   ( hap_cursor_button4.GetInt()==0 && IsButtonDown(4) ) );

			if(MouseMove!=m_bMovingCursor)
			{
				if(MouseMove)
				{
					//store the position of mouse and haptic axes.
						g_InputInternal->GetCursorPosition( m_iCursorStart[0] , m_iCursorStart[1] );
					m_flAxisStart[0] = m_flHapticsPosition[0];
					m_flAxisStart[1] = m_flHapticsPosition[1];
					// could do some sort of fancy effects here
					// OnStartCursorMove() ?
				}
				//else
				//	OnEndCursorMove() ?

				m_bMovingCursor = MouseMove;
			}
		}else{
			return false;// we were unsuccessful in reading the haptics.
		}
	}
	
	if(hap_cursor_absolute.GetInt()==1)
	{
		*x = ScreenWidth()/2.0+(m_flHapticsPosition[0])*ScreenWidth()*(hap_cursor_sensitivity.GetFloat()/2);;
		*y = ScreenHeight()/2.0+((-m_flHapticsPosition[1]))*ScreenHeight()*(hap_cursor_sensitivity.GetFloat()/2);;
	}else{
		// calculate the movement and multiply the new movement additive by the hap_cursor_sensitivity/2. 
		// sensitivity is divided by two because the axes go from -1 to +1 
		// (sensitivity does work off display resolution, is that a problem?)
		// Both X and Y work off the screen width to not squeeze the input by the screen ratio.
		//X adds
		*x = m_iCursorStart[0]+( m_flHapticsPosition[0] - m_flAxisStart[0] )*ScreenWidth()*(hap_cursor_sensitivity.GetFloat()/2);//(int) ( ( (float) m_flHapticsPosition[0]+1.0f ) * (float)ScreenWidth() ) \ 2.0f;
		//Y subtracts
		*y = m_iCursorStart[1]-( m_flHapticsPosition[1] - m_flAxisStart[1] )*ScreenWidth()*(hap_cursor_sensitivity.GetFloat()/2);//(int) ( ( (float) m_flHapticsPosition[0]+1.0f ) * (float)ScreenWidth() ) \ 2.0f;
	}
	// Comensate for the offscreen movement first by moving our start position in. then move our out position to the min or max.
	//X
	if(*x>ScreenWidth())
	{
		m_iCursorStart[0]-=(*x)-ScreenWidth();
		*x=ScreenWidth();
	}
	else if(*x<0)
	{
		m_iCursorStart[0]-=(*x);
		*x=0;
	}
	//Y
	if(*y>ScreenHeight())
	{
		m_iCursorStart[1]-=(*y)-ScreenHeight();
		*y=ScreenHeight();
	}
	else if(*y<0)
	{
		m_iCursorStart[1]-=(*y);
		*y=0;
	}

	return m_bMovingCursor;
}
// Quick helper for clearing movement buttons
void C_Haptics::ClearMoveButtons()
{
	int clearButtons = 0;
	const char* ignorekey = NULL;
	if( in_back.state & 1 )
	{
		KeyUp(&in_back, ignorekey);
		clearButtons |= IN_BACK;
	}
	if( in_forward.state & 1 )
	{
		KeyUp(&in_forward, ignorekey);
		clearButtons |= IN_FORWARD;
	}
	if( in_moveright.state & 1 )
	{
		KeyUp(&in_moveright, ignorekey);
		clearButtons |= IN_MOVERIGHT;
	}
	if( in_moveleft.state & 1 )
	{
		KeyUp(&in_moveleft, ignorekey);
		clearButtons |= IN_MOVELEFT;
	}
	if(clearButtons)
		input->ClearInputButton(clearButtons);
}

Vector C_Haptics::GetLastForceSent()
{
	if(hdlHaptics::instance())
	{
		etVector forceVect = hdlHaptics::instance()->getLastForceSent();
		return(Vector(forceVect.x(),forceVect.y(),forceVect.z()));
	}
	return Vector(0,0,0);
};

//-----------------------------------------------------------------------------
// Purpose: Apply haptics to CUserCmd creation
// Input  : frametime - 
//			*cmd - 
//-----------------------------------------------------------------------------

static bool s_bInGame = false;

//Pat: adding in pitch CVARS for looking up and down in vehicle
ConVar hap_vehicle_pitch_base("hap_vehicle_pitch_base","-20",FCVAR_ARCHIVE);
ConVar hap_vehicle_pitch_scale("hap_vehicle_pitch_scale","-38",FCVAR_ARCHIVE);
ConVar hap_vehicle_yaw_base("hap_vehicle_yaw_base","90",FCVAR_ARCHIVE);
ConVar hap_vehicle_yaw_scale("hap_vehicle_yaw_scale","-73",FCVAR_ARCHIVE);
void C_Haptics::HapticsMove( float frametime, CUserCmd *cmd )
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	bool bJustOutOfVehicle = false;
	if(m_bUpdateVehicle)
	{
		// this peice of code handles turning off vehicle mode. 
		// Currently the only way to get into vehicle mode is to call StartVehicleMode and entering the type.
		if(pPlayer && !pPlayer->IsInAVehicle())
		{
			ClearVehicleForces();
			m_bUpdateVehicle = false;
			bJustOutOfVehicle = true;
		}
		
	}


	// do not process if cursor is visible or the window is not in focus.
	if(InUIMode()/*||!WindowIsFocused()*/)
		return;

	// complete initialization if first time in (not already initialized)
	// ( needed as cvars are not available at initialization time ), is this true?
	if ( !m_fHapticsInit || !hdlHaptics::instance() )
		Init_Haptics();

	// verify haptics device is to be used
	if ( !in_haptics.GetInt() )
	{
		return; 
	}

	// collect the haptics data, if possible
	if ( !ReadHaptics() )
	{
		return;
	}
	// handle some debugging message printing.
	if ( hap_debug_axis.GetInt() == 1)
		Msg("Position (XYZ, absolute) - ");

	if ( hap_debug_axis.GetInt() == 2)
		Msg("Position (XYZ,   device) - ");

	QAngle viewangles;

	// Get starting angles
	engine->GetViewAngles( viewangles );
#ifdef PSIMOD
	//psimod does a little force calculation in the player class. a bit hacky
	C_HandPlayer *pMH_Player = C_HandPlayer::GetLocalHandPlayer();
	if(pMH_Player)
	{
		if(pMH_Player->HandleInput(this,frametime,viewangles))
		{
			engine->SetViewAngles(viewangles);
		}
	}
	if(m_tVelocityTimer.GetDurationInProgress().GetMilliseconds()>(unsigned int)hap_avatar_samplerate.GetInt())
	{
		Vector ScreenSpaceVelocity;
		VectorYawRotate(pPlayer->GetAbsVelocity(), -90 - pPlayer->GetAbsAngles()[YAW], ScreenSpaceVelocity );
		ProcessVelocitySample(ScreenSpaceVelocity*hap_avatar_scale.GetFloat());
		m_tVelocityTimer.Start();
	}
	return;
#endif

	struct axis_t
	{
		float	value;
		int		controlType;
	};
	axis_t gameAxes[ MAX_GAME_AXES ];
	memset( &gameAxes, 0, sizeof(gameAxes) );
	axis_t gameAxesRaw[ MAX_GAME_AXES ];
	memset( &gameAxesRaw, 0, sizeof(gameAxesRaw) );

	// Get each haptics axis value, and normalize the range
	for ( int i = 0; i < MAX_HAPTICS_AXES; ++i )
	{
		if ( GAME_AXIS_NONE == m_rgHapAxes[i].AxisMap )
			continue;

		double fAxisValue = clamp( m_flHapticsPosition[i], -1.0, 1.0 );

		if ( hap_debug_axis.GetInt() == 1)
			Msg("%+0.6f ", fAxisValue);
		if ( hap_debug_axis.GetInt() == 2)
			Msg("%+0.6f ", m_flRawHapticsPosition[i]);

		unsigned int idx = m_rgHapAxes[i].AxisMap;
		gameAxes[idx].value = fAxisValue;
		gameAxes[idx].controlType = m_rgHapAxes[i].ControlMap;

		gameAxesRaw[idx].value = m_flRawHapticsPosition[i];
		gameAxesRaw[idx].controlType = m_rgHapAxes[i].ControlMap;
	}

	if ( hap_debug_axis.GetInt() > 0)
#ifndef TABLE_ONLY
		Msg("- Health state: %+0.2f\n", hap_testint.GetFloat());
#else
		Msg("\n");
#endif

	// Going to borrow in_strafe for alt fire temporarily (will function for vehicle mode)
	if ( ( m_bUpdateVehicle ) && ( (in_strafe.state & 1) || lookstrafe.GetFloat() ) )
	{
		// user wants yaw control to become side control
		gameAxes[GAME_AXIS_SIDE] = gameAxes[GAME_AXIS_YAW];
		gameAxes[GAME_AXIS_YAW].value = 0;
	}

	// 1.0 replaced MAX_BUTTONSAMPLE
	float forward	= HapScaleAxisValue( gameAxes[GAME_AXIS_FORWARD].value, 1.0 * hap_forwardthreshold.GetFloat() );
	float side		= HapScaleAxisValue( gameAxes[GAME_AXIS_SIDE].value, 1.0 * hap_sidethreshold.GetFloat()  );
	float pitch		= HapScaleAxisValue( gameAxes[GAME_AXIS_PITCH].value, 0.0); // redundant?
	float yaw		= HapScaleAxisValue( gameAxes[GAME_AXIS_YAW].value, 0.0); // redundant?

	float	hapSideMove = 0.f;
	float	hapForwardMove = 0.f;

	// apply forward and side control
	hapForwardMove	+= ResponseCurve( hap_adv_response_move.GetInt(), forward ) * hap_forwardsensitivity.GetFloat() * cl_forwardspeed.GetFloat() *
		((hap_var_avatar_movement_mode.GetInt() == 1) ? 1.0 : 0.0);
	hapSideMove		+= ResponseCurve( hap_adv_response_move.GetInt(), side ) * hap_sidesensitivity.GetFloat() * cl_sidespeed.GetFloat();

	cmd->forwardmove += hapForwardMove;
	cmd->sidemove += hapSideMove;
	// storage only.
	Vector4D Axes(	gameAxes[GAME_AXIS_PITCH].value,
					gameAxes[GAME_AXIS_YAW].value,
					gameAxesRaw[GAME_AXIS_PITCH].value,
					gameAxesRaw[GAME_AXIS_YAW].value);
	
	// calculate our transion value ( so that when we exit the menu the view does not pop to the position change. )
	float tt = bJustOutOfVehicle ? 0 : gpGlobals->curtime - m_flTransitionTime;
	if(tt<hap_transition_time.GetFloat())
	{
		tt/=hap_transition_time.GetFloat();
	}else{
		tt = 1;
	}
	
	// calculate our view delta
	switch(hap_view_mode.GetInt())
	{
		case 1://regularly circle move, but not fully implemented.
			//viewangles += CircleMoveView(Axes, viewangles, pitch, yaw, frametime, cmd, tt);
			//break;
		default:
		case 0://square move
			viewangles += SquareMoveView(Axes, viewangles, pitch, yaw, frametime, cmd, tt);
			break;
	};


	// Check for haptic effects toggle code
	if(var_hap_togglehaptics == 1)
	{
		var_hap_togglehaptics = 0;
		if(in_haptics_effects.GetInt() != 1)
		{
			in_haptics_effects.SetValue(1);
			Msg("Haptics effects toggled on\n");
		}
		else
		{
			in_haptics_effects.SetValue(0);
			Msg("Haptics effects toggled off\n");
		}
	}

	// Check for movement toggle code
	if(var_hap_togglemovement == 1)
	{
		int tempMode;
		var_hap_togglemovement = 0;

		if (m_bUpdateVehicle)
		{
			Msg("Movement mode (vehicle) toggled\n");
			tempMode = hap_var_vehicle_movement_mode.GetInt() + 1;
			if(tempMode > 2)
				hap_var_vehicle_movement_mode.SetValue(1);
			else
				hap_var_vehicle_movement_mode.SetValue(tempMode);
		}
		else
		{
			Msg("Movement mode (avatar) toggled\n");
			tempMode = hap_var_avatar_movement_mode.GetInt() + 1;
			if(tempMode > 2)
				hap_var_avatar_movement_mode.SetValue(1);
			else
				hap_var_avatar_movement_mode.SetValue(tempMode);
		}

		bButtonCleared = false;
		bKeyboardTime = 0;
	}

	// Vehicle movement input
	if(m_bUpdateVehicle)
	{
		// Clear throttle and turn between movement mode change
		if(!bButtonCleared) 
		{ 
			ClearMoveButtons();
			bButtonCleared = true;
		}
		//Pat: adding pitch to vehicles - 7/18/07 Note:(tt is transition percent from vehicle entry)
		//Pat: adding yaw to vehicles - 8/15/07 Note:(tt is transition percent from vehicle entry)
		viewangles[PITCH] = hap_vehicle_pitch_base.GetFloat() + gameAxes[GAME_AXIS_PITCH].value*hap_vehicle_pitch_scale.GetFloat()*tt;
		viewangles[YAW] = hap_vehicle_yaw_base.GetFloat() + (gameAxes[GAME_AXIS_YAW].value*hap_vehicle_yaw_scale.GetFloat())*tt;
		const char* ignorekey = NULL;
		switch(hap_var_vehicle_movement_mode.GetInt())
		{
		case 4:// Just throttle.
		case 2:// Throttle and turn
			// Throttle
			if((forward > 0) && (abs(gameAxesRaw[GAME_AXIS_FORWARD].value) > hap_vehicle_threshold_throttle.GetFloat()))
			{	
				KeyDown( &in_back, ignorekey );
				KeyUp( &in_forward, ignorekey );
			}
			else if((forward < 0) && (abs(gameAxesRaw[GAME_AXIS_FORWARD].value) > hap_vehicle_threshold_throttle.GetFloat()))
			{	
				KeyUp( &in_back, ignorekey ); 
				KeyDown( &in_forward, ignorekey ); 
			}
			else
			{	
				KeyUp( &in_back, ignorekey ); 
				KeyUp( &in_forward, ignorekey );
			}
			if(hap_var_vehicle_movement_mode.GetInt()==4)
				break;
		case 1:
			// Turn
			if((yaw > 0) && (abs(gameAxesRaw[GAME_AXIS_YAW].value) > hap_vehicle_threshold_turn.GetFloat()))
			{ 
				KeyDown( &in_moveright, ignorekey );	
				KeyUp( &in_moveleft, ignorekey ); 
			}
			else if((yaw < 0) && (abs(gameAxesRaw[GAME_AXIS_YAW].value) > hap_vehicle_threshold_turn.GetFloat()))
			{ 
				KeyUp( &in_moveright, ignorekey ); 
				KeyDown( &in_moveleft, ignorekey ); 
			}
			else
			{ 
				KeyUp( &in_moveright, ignorekey ); 
				KeyUp( &in_moveleft, ignorekey ); 
			}
			break;
		case 3:
		default:
			break;
		}
	}else{
		// client side velocity sampling. This is a sample of how these effects can be handled client side.
		// vehicles could also be handled this way though its good to have a sample of how the networking can be
		// streamed.
		if(m_tVelocityTimer.GetDurationInProgress().GetMilliseconds()>(unsigned int)hap_avatar_samplerate.GetInt())
		{
			Vector ScreenSpaceVelocity;
			VectorYawRotate(pPlayer->GetAbsVelocity(), -90 - pPlayer->GetAbsAngles()[YAW], ScreenSpaceVelocity );
			ProcessVelocitySample(ScreenSpaceVelocity*hap_avatar_scale.GetFloat());
			m_tVelocityTimer.Start();
		}
	}
	// Bound pitch
	viewangles[PITCH] = clamp( viewangles[ PITCH ], -cl_pitchup.GetFloat(), cl_pitchdown.GetFloat() );

	// apply our modifications to the view.
	engine->SetViewAngles( viewangles );
}
QAngle C_Haptics::SquareMoveView( Vector4D Axes, QAngle viewangles, float pitch, float yaw, float frametime, CUserCmd *cmd, float transition, bool bFake )
{
	float   aspeed = frametime * gHUD.GetFOVSensitivityAdjust();
	Vector2D move( yaw, pitch );
	float dist = move.Length();
	// apply turn control
	float angle = 0.f;
	Vector2D gameAxes(Axes.x,Axes.y);
	Vector2D gameAxesRaw(Axes.z,Axes.w);
	QAngle Offset = QAngle(0,0,0);
	if( abs(gameAxesRaw[YAW]) >= hap_yawthreshold.GetFloat() && !bFake )
	{
		float fAxisValue;
		fAxisValue = HapScaleAxisValue( gameAxes[YAW], 1.0 * hap_yawthreshold.GetFloat()  );			
		float percent = 1.0f;
		if(hap_yaw_analog.GetInt()==1)
		{
			percent = min( ( abs(gameAxesRaw[YAW] ) - hap_yawthreshold.GetFloat() ) / ( 0.050f-hap_yawthreshold.GetFloat() ), 1.0f);
		}
		angle = fAxisValue * hap_yawsensitivity_velctrl.GetFloat() * aspeed * cl_yawspeed.GetFloat() * percent *
			( (m_bUpdateVehicle) && !(in_hap_holdcamera.state & 1) ? 0.0 : 1.0);
	}
	else
	{
		float fAxisValue = ResponseCurveLook( hap_adv_response_look.GetInt(), yaw, YAW, pitch, dist );
		if((int)(g_yawData.Count()) > hap_axis_relative_samples.GetInt()&&!bFake)
		{
			if(!bFake)
			{
				// Remove extra data from queue (pop)
				g_averageYawValue -= g_yawData.Element(0);
				g_yawData.Remove(0);
			}
		}
		else if((int)(g_yawData.Count()) < hap_axis_relative_samples.GetInt())
		{
			if(!bFake)
			{
				// Fill the sample queue (push)
				g_yawData.AddToTail(fAxisValue / (float)(hap_axis_relative_samples.GetInt()));
				g_averageYawValue += fAxisValue / (float)(hap_axis_relative_samples.GetInt());
			}
		}
		else
		{
			float AverageYawValue;
			if(!bFake)
			{
				// Sample queue is full (pop then push)
				g_averageYawValue -= g_yawData.Element(0);
				g_yawData.Remove(0);
				g_yawData.AddToTail(fAxisValue / (float)(hap_axis_relative_samples.GetInt()));
				g_averageYawValue += fAxisValue / (float)(hap_axis_relative_samples.GetInt());
				AverageYawValue = g_averageYawValue;

			}else{
				AverageYawValue = ( g_averageYawValue - g_yawData.Element(0) ) + fAxisValue / (float)(hap_axis_relative_samples.GetInt());
			}
			if((!(in_hap_holdcamera.state & 1) || (m_bUpdateVehicle) ))//&&(hap_view_broken.GetInt()==0||bFake))
			angle = ((fAxisValue - AverageYawValue ) * hap_yawsensitivity_posctrl.GetFloat()) * aspeed * 180.0 *
				((m_bUpdateVehicle) && !(in_hap_holdcamera.state & 1) ? 0.0 : 1.0)*transition;
		}
	}
	Offset[YAW] += angle;
	if(cmd)
		cmd->mousedx = angle;

	// apply look control
	if ( IsXbox() || in_jlook.state & 1 )
	{
		float angle = 0;
		//if ( abs(pitch) >= hap_pitchthreshold_outer.GetFloat() )
		if( abs(gameAxesRaw[PITCH]) >= hap_pitchthreshold.GetFloat() && !bFake )
		{
			float fAxisValue;
			fAxisValue = HapScaleAxisValue( gameAxes[PITCH], 1.0 * hap_pitchthreshold.GetFloat()  );
			float percent = 1.0f;
			if(hap_pitch_analog.GetInt()==1)
			{
				percent = min( ( abs(gameAxesRaw[PITCH] ) - hap_pitchthreshold.GetFloat() ) / ( 0.050f-hap_pitchthreshold.GetFloat() ), 1.0f);
			}
			angle = percent * fAxisValue * hap_pitchsensitivity_velctrl.GetFloat() * aspeed * cl_pitchspeed.GetFloat() *
				((m_bUpdateVehicle) && !(in_hap_holdcamera.state & 1) ? 0.0 : 1.0);
		}
		else
		{
			float fAxisValue = ResponseCurveLook( hap_adv_response_look.GetInt(), pitch, PITCH, yaw, dist );
			if((int)(g_pitchData.Count()) > hap_axis_relative_samples.GetInt())
			{
				if(!bFake)
				{
					// Remove extra data from queue (pop)
					g_averagePitchValue -= g_pitchData.Element(0);
					g_pitchData.Remove(0);
				}
			}
			else if((int)(g_pitchData.Count()) < hap_axis_relative_samples.GetInt())
			{
				if(!bFake)
				{
					// Fill the sample queue (push)
					g_pitchData.AddToTail(fAxisValue / (float)(hap_axis_relative_samples.GetInt()));
					g_averagePitchValue += fAxisValue / (float)(hap_axis_relative_samples.GetInt());
				}
			}
			else
			{
				float AveragePitchValue;
				if(!bFake)
				{
					// Sample queue is full (pop then push)
					g_averagePitchValue -= g_pitchData.Element(0);
					g_pitchData.Remove(0);
					g_pitchData.AddToTail(fAxisValue / (float)(hap_axis_relative_samples.GetInt()));
					g_averagePitchValue += fAxisValue / (float)(hap_axis_relative_samples.GetInt());
					AveragePitchValue = g_averagePitchValue;
				}else{
					AveragePitchValue = (g_averagePitchValue-g_pitchData.Element(0)) + fAxisValue / (float) hap_axis_relative_samples.GetInt();
				}
				// Consider inner and outer sensitivities later
				if((!(in_hap_holdcamera.state & 1) || (m_bUpdateVehicle) ))
					angle = ((fAxisValue - AveragePitchValue) * hap_pitchsensitivity_posctrl.GetFloat()) * aspeed * 180.0 *
						((m_bUpdateVehicle) && !(in_hap_holdcamera.state & 1) ? 0.0 : 1.0) * transition;
			}
		}
		Offset[PITCH] += angle;
		if(cmd)
			cmd->mousedy = angle;
		view->StopPitchDrift();
		
		if( pitch == 0.f && lookspring.GetFloat() == 0.f )
		{
			// no pitch movement
			// disable pitch return-to-center unless requested by user
			// *** this code can be removed when the lookspring bug is fixed
			// *** the bug always has the lookspring feature on
			view->StopPitchDrift();
		}
	}
	Offset[ROLL] = 0;
	return Offset;
}

QAngle C_Haptics::GetPointAngle(float fov)
{
	QAngle out = QAngle(0,0,0);
	if(!InUIMode())
	{	
		float ratio = (float) ScreenWidth() / (float) ScreenHeight();
		fov /= ratio;
		out[PITCH] = m_flHapticsPosition[1]*hap_pitchsensitivity_posctrl.GetFloat()/2;
		out[YAW]   = m_flHapticsPosition[0]*hap_yawsensitivity_posctrl.GetFloat()/2;
	}
	out[ROLL] = 0;
	return out;
}

// Haptics --------------------------------------------------------------------
// Purpose: Resets velocity samples 
//-----------------------------------------------------------------------------
void C_Haptics::ResetSamples()
{
	m_VelocityQueue.RemoveAll();
}

// Haptics --------------------------------------------------------------------
// Purpose: Clears avatar force effects 
//-----------------------------------------------------------------------------
void C_Haptics::ClearAvatarForces()
{
	//clear the effect in the haptics instance.
	hdlHaptics::instance()->triggerEffect( Avatar,
			0.0, 0.0, 0.0, 0.0, 0, 0);
	ResetSamples();
};

// Haptics --------------------------------------------------------------------
// Purpose: Clears vehicle force effects
//-----------------------------------------------------------------------------
void C_Haptics::ClearVehicleForces()
{
	//clear the effect in the haptics instance.
	if(hdlHaptics::instance())
		hdlHaptics::instance()->triggerEffect( Vehicle,
				0.0, 0.0, 0.0, 0.0, 0, 0);
	ResetSamples();
	ClearMoveButtons();
};

#define HAP_VELOCITY_SAMPLES 3
// Haptics --------------------------------------------------------------------
// Purpose: Processes a new sample of velocity into the current velocity
// required effect. (vehicle or avatar)
//-----------------------------------------------------------------------------
void C_Haptics::ProcessVelocitySample( Vector Velocity )
{
	// return if effects are off.
	if(in_haptics_effects.GetInt() == 0)
		return;

	// Stop the clock.
	m_tVelocityTimer.End();

	// average the duration into the average delta time.
	m_flAverageDeltaTime += (float)m_tVelocityTimer.GetDuration().GetSeconds();
	m_flAverageDeltaTime /= 2.0;
	
	Vector vDeltaAcceleration;
	vDeltaAcceleration.Init();

	int vVelQueueStartSize = m_VelocityQueue.Count();
	
	if( vVelQueueStartSize != HAP_VELOCITY_SAMPLES)
	{
		if(hap_debug_vehicle.GetInt()==-1)
			Msg("samples = %i to ",m_VelocityQueue.Count());
		// if we do not have enough samples add our velocity.
		m_VelocityQueue.AddToTail(Velocity);
		if(hap_debug_vehicle.GetInt()==-1)
			Msg("%i\n",m_VelocityQueue.Count());
		// if this is our first sample we assume that we started from zero.
	}
	else
	{
		// we have all sample slots full and we want this new sample too, so lets remove the oldest sample
		m_VelocityQueue.Remove(0);
		// and add our new sample.
		m_VelocityQueue.AddToTail(Velocity);
	}
	// get the difference between our now oldest sample to our new sample.
	//for( int i = 0; i != m_VelocityQueue.Size()-1; i++ )
	{
		// for each sample that has a sample higher than them
		// add the difference to the higher sample
		vDeltaAcceleration += m_VelocityQueue[m_VelocityQueue.Size()-1]-m_VelocityQueue[0];

		// if we are a nonzero number divide by 2.
		//vDeltaAcceleration /= i ? 2 : 1;
	}

	Vector vForce = vDeltaAcceleration * METERS_PER_INCH / (2*m_flAverageDeltaTime);

	if(m_bUpdateVehicle) // Vehicle
	{
		if(hap_debug_vehicle.GetInt() == 1)
		{
			Msg("Vehicle: Acceleration (m/s^2) = %+0.4f, %+0.4f, %+0.4f - dT = %0.2f [ %s ]\n", // - 2nd Der = ",
				vForce.x, vForce.y, vForce.z, m_flAverageDeltaTime);
		}

		if(VehicleForceThreshold)
			// apply vehicle thresholding.
			vForce = VehicleForceThreshold->ProcessVector(vForce);
		else
			// theres no specific vehicle thresholding so go to our default.
			vForce = ( (ThresholdInfo *)&s_ThresholdAvatar )->ProcessVector(vForce);

		vForce *=hap_vehicle_force_scale.GetFloat();

		hdlHaptics::instance()->triggerEffect( Vehicle,
			1.0, vForce.x, vForce.z, -vForce.y, (int)(m_flAverageDeltaTime * 1000.0), 0);

	}
	else //if(m_bUpdateAvatar) // Avatar
	{	
		//testing 
		if(CBasePlayer::GetLocalPlayer()&&CBasePlayer::GetLocalPlayer()->IsInAVehicle())
		{
			Warning("In a vehicle and not processing so!\n");
		}
		if(hap_avatar_disable.GetInt()==0)
		{
			if(hap_debug_avatar.GetInt() == 1)
				Msg("Avatar: Acceleration (m/s^2) = %+0.4f, %+0.4f, %+0.4f - dT = %0.2f\n",
				vForce.x, vForce.y, vForce.z, m_flAverageDeltaTime);

			// Apply avatar thresholding.
			vForce = ( (ThresholdInfo *)&s_ThresholdAvatar )->ProcessVector(vForce);
			
			hdlHaptics::instance()->triggerEffect( Avatar,
				1.0, vForce.x, -vForce.z, -vForce.y, (int)(m_flAverageDeltaTime * 1000.0), 0);
		}
	}

	//start the timer again for our next sample.
	m_tVelocityTimer.Start();


}
// for debug lookup:
static const char *s_StepTypeNames[]=
{
	"Solid",//STEPTYPE_SOLID = 0
	"Splashing",//STEPTYPE_SPLASHING = 1
	"Wading",//STEPTYPE_WADING = 2
};

// Haptics --------------------------------------------------------------------
// Purpose: Sends a footstep effect to the haptics device.
//-----------------------------------------------------------------------------
void C_Haptics::DoFootstep(StepType_t type, float strength)
{
	// return if effects are off.
	if(in_haptics_effects.GetInt() == 0)
		return;

	// initialize local variables.
	int time = 1;
	float peak = 1;

	// multiply strength and fill in time and peak variables based on the type.
	switch(type)
	{
		case STEPTYPE_SPLASHING:
			strength *= hap_step_scale_splash.GetFloat();
			time = hap_step_time_splash.GetInt();
			peak = hap_step_peakpct_splash.GetFloat();
			break;

		case STEPTYPE_WADING:
			strength *= hap_step_scale_wade.GetFloat();
			time = hap_step_time_wade.GetInt();
			peak = hap_step_peakpct_wade.GetFloat();
			break;

		default:
			// if not in this list set type to standard.
			type = STEPTYPE_SOLID;
			strength *= hap_step_scale_regular.GetFloat();
			time = hap_step_time_regular.GetInt();
			peak = hap_step_peakpct_regular.GetFloat();
			break;
	}
	
	if(hap_debug_steps.GetInt() == 1)
		Msg("Footsteps detected: Strength = %+0.2f (%s)\n", strength, s_StepTypeNames[type] );

	hdlHaptics::instance()->triggerEffect( Footsteps,
		strength,
		0.0, //hap_var_effectstep_x.GetFloat(),
		1.0, //hap_var_effectstep_y.GetFloat(),
		0.0, //hap_var_effectstep_z.GetFloat(),
		time,
		peak
		);
}
// Haptics --------------------------------------------------------------------
// Purpose: Sends a damage to the haptics device. pitch and yaw is the direction of the shot. 
//-----------------------------------------------------------------------------
void C_Haptics::DoDamageSimulation(float pitch, float yaw, float damage)
{
	// return if effects are off.
	if(in_haptics_effects.GetInt() == 0)
		return;
	
	pitch*=-1.0f;
	hdlHaptics::instance()->triggerEffect( Damage,
		damage * hap_damage_scale.GetFloat(),
		cos(pitch*M_PI/180.0)*sin(yaw*M_PI/180.0),
		sin(pitch*M_PI/180.0),
		(cos(pitch*M_PI/180.0)*cos(yaw*M_PI/180.0)),
		hap_damage_time.GetInt(), 
		hap_damage_peakpct.GetFloat());

	if(hap_debug_damage.GetInt() == 1)
		Msg("Damage detected: Pitch = %+0.2f, Yaw = %+0.2f, Amount = %0.2f\n", pitch, yaw, damage);
	

}


// Haptics --------------------------------------------------------------------
// Purpose: Sends recoil effect to the haptics device.
//-----------------------------------------------------------------------------
void C_Haptics::DoRecoil( QAngle angle, float strength)
{
	// return if effects are off.
	if(in_haptics_effects.GetInt() == 0)
		return;

	if(hap_debug_recoil.GetInt() == 1)
		Msg("Recoil detected: MagS = %+0.2f, XS = %+0.2f, YS = %+0.2f, ZS = %+0.2f, AngleS = %+0.2f, DurS = %+0.2d, RampS = %+0.2f\n",
			strength * hap_recoil_scale.GetFloat(),
			angle[PITCH],
			angle[YAW],
			angle[ROLL],
			hap_recoil_scale_angle.GetFloat(),
			hap_recoil_time.GetInt(), 
			hap_recoil_peakpct.GetFloat());
	//Send the effect to the haptics instance.
	hdlHaptics::instance()->triggerEffect(Recoil,
		strength * hap_recoil_scale.GetFloat(),
		angle[PITCH] * hap_recoil_scale_angle.GetFloat(),
		angle[YAW] * hap_recoil_scale_angle.GetFloat(),
		angle[ROLL] * hap_recoil_scale_angle.GetFloat(),
		hap_recoil_time.GetInt(), hap_recoil_peakpct.GetFloat());

}

Vector C_Haptics::GetLimitedInputAxes()
{
	Vector v = GetInputAxes();
	if(v.Length()>1.0f)
	{
		VectorNormalize(v);
	}
	return v;
}
// Haptics --------------------------------------------------------------------
// Purpose: Changes the mass simulation amount. Disables effect if set to zero.
//-----------------------------------------------------------------------------
void C_Haptics::SetHeldMass(float mass)
{
	// return if effects are off.
	if(in_haptics_effects.GetInt() == 0)
		return;

	// only update if we are currently not at that mass.
	if( ( m_flCurrentHeldMass != mass))
	{
		if(hap_debug_carry.GetInt() == 1)
			Msg("Physics (carry) updated: Weight = %+0.2f\n", mass);

		if( mass != 0.0f )
		{
			// if we are applying or reducing held mass.
			hdlHaptics::instance()->triggerEffect(Carry, mass * hap_carry_scale.GetFloat(),
				0,0,0,
				0,0);			
		}else{
			// we are removing all mass.
			hdlHaptics::instance()->terminateEffect(Carry);
		}

		m_flCurrentHeldMass = mass;
	}
}

// Haptics --------------------------------------------------------------------
// Purpose: Calculates what mode we are currently in.
//-----------------------------------------------------------------------------
int C_Haptics::GetCurrentMode()
{
	// if we are updating a vehicle:
	if(m_bUpdateVehicle)
		// we are in vehicle mode.
		return HAP_MODE_VEHICLE;
	// if we got to this point we are not in any special processing mode.
	return HAP_MODE_STANDARD;
}
// for testing
void C_Haptics::TriggerEffect(int a_effectID, float a_effectAmplitude,
		float a_x, float a_y, float a_z, int a_effectDuration, float a_effectRampUp,
		float b_x, float b_y, float b_z)
{
	if(hdlHaptics::instance())
		hdlHaptics::instance()->triggerEffect((EffectType)a_effectID, a_effectAmplitude,
											  a_x,a_y,a_z,a_effectDuration,a_effectRampUp,
											  b_x,b_y,b_x);
}
#ifdef HAPTICS_NO_BASE
//global
static C_Haptics g_Haptics;
//external access.
C_Haptics *cliHaptics = &g_Haptics;
#endif
// for button checking.
CON_COMMAND(hap_debug_printbuttons,"Prints button states")
{
	if(!cliHaptics)
		return;
	Msg("==BUTTONS==\n");
	for(int i = 0;i != 4;i++)
	{
		Msg("%i is %s.\n",i+1,cliHaptics->IsButtonDown(i+1)?"down":"up");
	}
}
// shuts down haptic device
CON_COMMAND(hap_shutdown,"Shuts down haptic device")
{
	if(!cliHaptics)
		return;
	Msg("Shutting down haptics..\n");
	cliHaptics->Shutdown_Haptics();
}
//checks if we are in UI mode.
bool InUIMode()
{
	return(	engine->IsPaused()||
			engine->IsLevelMainMenuBackground()||
			vgui::surface()->IsCursorVisible()||
			!engine->IsInGame());
}

// converts between haptic coordinate and source coordinate
void HapticToSource(Vector &hapcoords)
{
	Vector a = hapcoords;
	hapcoords.x = -a.z;
	hapcoords.y = a.x;
	hapcoords.z = a.y;
}
void SourceToHaptic(Vector &sourcecoords)
{
	Vector a = sourcecoords;
	sourcecoords.x = a.y;
	sourcecoords.y = a.z;
	sourcecoords.z = -a.x;
}