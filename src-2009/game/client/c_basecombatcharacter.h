//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Defines the client-side representation of CBaseCombatCharacter.
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_BASECOMBATCHARACTER_H
#define C_BASECOMBATCHARACTER_H

#ifdef _WIN32
#pragma once
#endif

#include "shareddefs.h"
#include "c_baseflex.h"

class C_BaseCombatWeapon;
class C_WeaponCombatShield;

class C_BaseCombatCharacter : public C_BaseFlex
{
	DECLARE_CLASS( C_BaseCombatCharacter, C_BaseFlex );
public:
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

					C_BaseCombatCharacter( void );
	virtual			~C_BaseCombatCharacter( void );

	virtual bool	IsBaseCombatCharacter( void ) { return true; };
	virtual C_BaseCombatCharacter *MyCombatCharacterPointer( void ) { return this; }

	// -----------------------
	// Ammo
	// -----------------------
	void				RemoveAmmo( int iCount, int iAmmoIndex );
	void				RemoveAmmo( int iCount, const char *szName );
	void				RemoveAllAmmo( );
	int					GetAmmoCount( int iAmmoIndex ) const;
	int					GetAmmoCount( char *szName ) const;

	C_BaseCombatWeapon*	Weapon_OwnsThisType( const char *pszWeapon, int iSubType = 0 ) const;  // True if already owns a weapon of this class
	virtual	bool		Weapon_Switch( C_BaseCombatWeapon *pWeapon, int viewmodelindex = 0 );
	virtual bool		Weapon_CanSwitchTo(C_BaseCombatWeapon *pWeapon);
	
	// I can't use my current weapon anymore. Switch me to the next best weapon.
	bool SwitchToNextBestWeapon(C_BaseCombatWeapon *pCurrent);

	virtual C_BaseCombatWeapon	*GetActiveWeapon( void ) const;
	int					WeaponCount() const;
	C_BaseCombatWeapon	*GetWeapon( int i ) const;

	// This is a sort of hack back-door only used by physgun!
	void SetAmmoCount( int iCount, int iAmmoIndex );

	float				GetNextAttack() const { return m_flNextAttack; }
	void				SetNextAttack( float flWait ) { m_flNextAttack = flWait; }

	virtual int			BloodColor();

	// Blood color (see BLOOD_COLOR_* macros in baseentity.h)
	void SetBloodColor( int nBloodColor );

	virtual void DoMuzzleFlash();

	CHandle< C_BaseCombatWeapon > m_hActiveWeapon;

public:

	float			m_flNextAttack;


protected:

	int			m_bloodColor;			// color of blood particless


private:
	CNetworkArray( int, m_iAmmo, MAX_AMMO_TYPES );

	CHandle<C_BaseCombatWeapon>		m_hMyWeapons[MAX_WEAPONS];
	//CHandle< C_BaseCombatWeapon > m_hActiveWeapon;

	friend class CShowWeapon;

private:
	C_BaseCombatCharacter( const C_BaseCombatCharacter & ); // not defined, not accessible


//-----------------------
#ifdef INVASION_CLIENT_DLL
public:
	virtual void	Release( void );
	virtual void	SetDormant( bool bDormant );
	virtual void	OnPreDataChanged( DataUpdateType_t updateType );
	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	ClientThink( void );

	// TF2 Powerups
	virtual bool	CanBePoweredUp( void ) { return true; }
	bool			HasPowerup( int iPowerup ) { return ( m_iPowerups & (1 << iPowerup) ) != 0; };
	virtual void	PowerupStart( int iPowerup, bool bInitial );
	virtual void	PowerupEnd( int iPowerup );
	void			RemoveAllPowerups( void );

	// Powerup effects
	void			AddEMPEffect( float flSize );
	void			AddBuffEffect( float flSize );

	C_WeaponCombatShield		*GetShield( void );

public:
	int				m_iPowerups;
	int				m_iPrevPowerups;
#endif

};

inline C_BaseCombatCharacter *ToBaseCombatCharacter( C_BaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsBaseCombatCharacter() )
		return NULL;

#if _DEBUG
	return dynamic_cast<C_BaseCombatCharacter *>( pEntity );
#else
	return static_cast<C_BaseCombatCharacter *>( pEntity );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline int	C_BaseCombatCharacter::WeaponCount() const
{
	return MAX_WEAPONS;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : i - 
//-----------------------------------------------------------------------------
inline C_BaseCombatWeapon *C_BaseCombatCharacter::GetWeapon( int i ) const
{
	Assert( (i >= 0) && (i < MAX_WEAPONS) );
	return m_hMyWeapons[i].Get();
}

EXTERN_RECV_TABLE(DT_BaseCombatCharacter);

#endif // C_BASECOMBATCHARACTER_H
