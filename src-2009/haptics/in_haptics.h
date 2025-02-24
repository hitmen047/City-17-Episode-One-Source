#ifndef IN_HAPTICS_H
#define IN_HAPTICS_H
#ifdef _WIN32
#pragma once
#endif
//#include "cli_in_haptics.h"

const bool HAPTICS_INIT_ON_STARTUP = false;

enum SpecialProcessingModes
{
	HAP_MODE_STANDARD = 0,
	HAP_MODE_VEHICLE,
};

//enumeration for step types.
enum StepType_t {
	STEPTYPE_SOLID = 0,
	STEPTYPE_SPLASHING,
	STEPTYPE_WADING,
	STEPTYPE_SWIMMING };

// Forward declaration
struct ThresholdInfo;

// Haptics --------------------------------------------------------------------
// C_Haptics is our client(haptics) interface to the servo hdlHaptics 
// class.
//
// the client's(haptics) job is to sync data with the servo's data 
// which runs and is calculated at a target of 1000 times a second (or 1ms). 
// the client(haptics) class pulls the data of the most recent servo
// calculated input data (handle position and buttons) and also updates/sends
// effects to the servo loop.
//
// when dealing with effects it is important to ensure that the majority of the
// data calculation is being done not here on the client(haptics) class but 
// instead on the servo loop. 

// Effects here should only be sent/started to the servo but there 
// are some cases where updating constantly changing effects that rely on data
// that is game specific data (eg. player movement) and that should be the only
// case where a effect is actually being updated on the client(haptics) class.
// Ideally there should be built in filtering on the effect class for these
// sorts to make the forces interpolate/smooth or else the force outputs could 
// feel very jittery and hard to control(unless of course that is the targeted
// effect).
//
// The main thing to remember is everything on C_Haptics is called with the
// game loop. the function C_Haptics::HapticsMove() is where updating occurs.
// C_Haptics::HapticsMove() is called from input function 
// CInput::ControllerMove() .
//
//-----------------------------------------------------------------------------

class C_Haptics
{

public:
	//constructor
				C_Haptics();
	//destructor
				~C_Haptics();

	// Startup/shutdown routines
	void		Init_Haptics( void );
	void		Shutdown_Haptics( void );
	void		Set_Init( bool bstate );
	// called on level initialization.
	void		LevelInit( void );

	// Device output functions (position/button states)
	virtual void		HapticsMove( float frametime, CUserCmd *cmd );
	
	// returns force values last sent to haptic device.
	Vector		GetLastForceSent();
	// get handle position (-1 to +1 but can go over or below based on calibration)
	// (its important to remember that the axes are not in source's coordinate scheme)
	// x = right | y = up | z = backwards | to convert to source..| x = -z | y = x | z = y |
	Vector		GetInputAxes(){return Vector(m_flHapticsPosition[0], m_flHapticsPosition[1], m_flHapticsPosition[2]);};
	// get handle position with a max length of 1
	Vector		GetLimitedInputAxes();

	// Impulse force related functions

	// Do footstep effect (steptype and strength)
	virtual void		DoFootstep(StepType_t surface, float vol);
	// Do recoil effect (direction and strength)
	virtual void		DoRecoil(QAngle angle,float strength);
	// Do directional damage effect ( 2D direction(pitch, yaw) and damage inflicted)
	virtual void		DoDamageSimulation(float pitch, float yaw, float damage);
	// Sets and updates held mass value. Set to zero to turn off effect.
	virtual void		SetHeldMass(float mass);

	// Process a new velocity sample.
	void		ProcessVelocitySample(Vector Velocity);
	// Resets velocity sample history.
	void		ResetSamples();
	// Clears avatar forces
	void		ClearAvatarForces();
	// Clears vehicle forces
	void		ClearVehicleForces();
	// Calculates what sort of special sampling we are currently doing.
	int			GetCurrentMode();
	
	// For debug usage only, make a interface or function to implement new effect.
	void		TriggerEffect(int a_effectID, float a_effectAmplitude=0,
		float a_x=0, float a_y=0, float a_z=0, int a_effectDuration=0, float a_effectRampUp=0,
		float b_x=0, float b_y=0, float b_z=0);

	// Sets x and y to simulated cursor position. returns true if mouse should be moving.
	bool		GetCursorPosition(int *x,int *y);

	// returns calculated additive angle based on handle (used for broken crosshair)
	QAngle		GetPointAngle(float fov);

	// checks if a specific button (1-4) is down.
	bool		IsButtonDown(int Number);

protected:
	void		ClearMoveButtons();

private:
	// private access. is called by ProcessVelocitySample if not in vehicle mode.
	void		ProcessAvatarSample(Vector Velocity);
	// private access. is called by ProcessVelocitySample if in vehicle mode.
	void		ProcessVehicleSample(Vector Velocity);
	// private access. is called by StopVelocityMode if in vehicle mode.
	void		StopVehicleMode();


	ButtonCode_t m_TempButton;
	// view angle calculations:

	// for yaw / pitch seperate calculation
	QAngle		SquareMoveView( Vector4D axis, QAngle current, float pitch, float yaw, float frametime, CUserCmd *cmd, float transition, bool bFake = false );
	// for direct angle calculation based on distance from center.
	QAngle		CircleMoveView( Vector4D axis, QAngle current, float pitch, float yaw, float frametime, CUserCmd *cmd );


private:
	float		HapScaleAxisValue( const float axisValue, const float axisThreshold );
	char const	*HapDescribeAxis( int index );
	// handles button updating.
	void		HapButtonEvent( int bstate );
	// updates haptics data, returns true if successful.
	bool	ReadHaptics ( void );

private:
	enum
	{
		GAME_AXIS_NONE = 0,
		GAME_AXIS_FORWARD,
		GAME_AXIS_PITCH,
		GAME_AXIS_SIDE,
		GAME_AXIS_YAW,
		MAX_GAME_AXES
	};

	enum HapticsAxis_t
	{
		HAP_AXIS_X = 0,
	 	HAP_AXIS_Y,
		HAP_AXIS_Z,
		HAP_AXIS_R,
		HAP_AXIS_U,
		HAP_AXIS_V,
		MAX_HAPTICS_AXES,
	};

	enum Model_t
	{
		LINEAR_SHORT = 0,
		LINEAR_MEDIUM,
		LINEAR_LONG,
		EXPONENTIAL_QUICK,
		EXPONENTIAL_LONG,
	};

	typedef struct
	{
		unsigned int AxisFlags;
		unsigned int AxisMap;
		unsigned int ControlMap;
	} hap_axis_t;

	int			m_nHapticsOldButtons;
	hap_axis_t	m_rgHapAxes[ MAX_HAPTICS_AXES ];

	int			m_bRecoilOn;
	float		m_fRecoilForce;
	float		m_fRecoilPitch;
	float		m_fRecoilYaw;

	bool		m_fHapticsInit;
	float		m_flHapticsPosition[ MAX_HAPTICS_AXES ];
	float		m_flRawHapticsPosition[ MAX_HAPTICS_AXES ];
	int			m_nHapticsButtons;
	int			m_nHapticsButtonPressed;
protected:
	ThresholdInfo	   *VehicleForceThreshold;
	bool				m_bUpdateVehicle;
private:
	CFastTimer			m_tVelocityTimer;
	// used to smoothly translate input under certain conditions.
	float				m_flTransitionTime;
	float				m_flAverageDeltaTime;
	bool				m_bUpdateAvatar;
	CUtlVector<Vector>	m_VelocityQueue;
	float				m_flTargetHeldMass;
	float				m_flCurrentHeldMass;
	// UI Cursor
	bool				m_bMovingCursor;
	int					m_iCursorStart[2];
	float				m_flAxisStart[2];

};

extern C_Haptics *cliHaptics;

//helper functions to convert haptic space to source space.
void HapticToSource(Vector &hapcoords);
void SourceToHaptic(Vector &hapcoords);
// static function, returns true if window has focus
//bool WindowIsFocused();
bool InUIMode();
#endif // IN_HAPTICS_H