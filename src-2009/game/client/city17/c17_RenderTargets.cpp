//========= Copyright � 1996-2008, Gear Software, All rights reserved. ============//
//
// Purpose: C17's Various Render Targets
//
// Credit: Also in this file is the RTs needed for our scopes, which come from The New Era Team.
// $NoKeywords: $
//=================================================================================//
#include "cbase.h"
#include "c17_RenderTargets.h"
#include "materialsystem\imaterialsystem.h"
#include "rendertexture.h"
 
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ITexture* CC17RenderTargets::CreateScopeTexture( IMaterialSystem* pMaterialSystem, int iSize )
{
	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		"_rt_Scope",
		iSize, iSize, RT_SIZE_OFFSCREEN,
		pMaterialSystem->GetBackBufferFormat(),
		MATERIAL_RT_DEPTH_SHARED, 
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_NOLOD,
		CREATERENDERTARGETFLAGS_HDR );
  
}

/*ITexture* CC17RenderTargets::CreateSunTexture( IMaterialSystem* pMaterialSystem, int iSize )
{
	return pMaterialSystem->CreateNamedRenderTargetTextureEx2(
		"_rt_SunShaftBlack",
		iSize, iSize, RT_SIZE_HDR,
		pMaterialSystem->GetBackBufferFormat(),
		MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_NOLOD,
		CREATERENDERTARGETFLAGS_HDR );

}*/

//-----------------------------------------------------------------------------
// Purpose: Called by the engine in material system init and shutdown.
//			Clients should override this in their inherited version, but the base
//			is to init all standard render targets for use.
// Input  : pMaterialSystem - the engine's material system (our singleton is not yet inited at the time this is called)
//			pHardwareConfig - the user hardware config, useful for conditional render target setup
//-----------------------------------------------------------------------------
void CC17RenderTargets::InitClientRenderTargets( IMaterialSystem* pMaterialSystem, IMaterialSystemHardwareConfig* pHardwareConfig )
{
	// Initialize the Scope.
	m_ScopeTexture.Init( CreateScopeTexture( pMaterialSystem ) );
	//m_SunShaftBlackTexture.Init( CreateSunTexture( pMaterialSystem ) );

	// Water effects & camera from the base class (standard HL2 targets) 
	BaseClass::InitClientRenderTargets( pMaterialSystem, pHardwareConfig );
}
  
//-----------------------------------------------------------------------------
// Purpose: Shut down each CTextureReference we created in InitClientRenderTargets.
//			Called by the engine in material system shutdown.
// Input  :  - 
//-----------------------------------------------------------------------------
void CC17RenderTargets::ShutdownClientRenderTargets()
{
	// Shut down the Scope.
	m_ScopeTexture.Shutdown();
	//m_SunShaftBlackTexture.Shutdown();

	// Clean up standard HL2 RTs (camera and water) 
	BaseClass::ShutdownClientRenderTargets();
}
 
//add the interface!
static CC17RenderTargets g_C17RenderTargets;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CC17RenderTargets, IClientRenderTargets, CLIENTRENDERTARGETS_INTERFACE_VERSION, g_C17RenderTargets  );
CC17RenderTargets* C17RenderTargets = &g_C17RenderTargets;
