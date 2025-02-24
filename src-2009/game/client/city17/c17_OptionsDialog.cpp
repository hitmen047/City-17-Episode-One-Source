//========= Copyright � 2009, Team Gear & Nigredo Studios, All rights reserved. ============//
//
// Purpose: City 17's options dialog, built from NightFall code, originally created by Matt Stafford. (And a little something else as well)
//
//==========================================================================================//

#include "cbase.h"
#include "c17_OptionsDialog.h"
//#include "FileSystem.h"
#include "KeyValues.h"
#include "ienginevgui.h"

#include "c17_OptionsSubVideo.h"
#include "c17_OptionsSubGameplay.h"
//#include "c17_OptionsSubNovint.h"
#include "c17_OptionsSubFaceAPI.h"
//#include "c17_OptionsSubMaxsi.h"

#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Label.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui/ILocalize.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

static CC17OptionsDialog *g_pC17OptionsDialog = NULL;

CON_COMMAND(OpenC17OptionsDialog, "Opens the City17 Options Dialog")
{
	if ( !g_pC17OptionsDialog )
	{
		g_pC17OptionsDialog = new CC17OptionsDialog( NULL );
		g_pC17OptionsDialog->Run();
		g_pC17OptionsDialog->SetVisible( false );
	}

	if ( !g_pC17OptionsDialog->IsVisible() )
	{
		g_pC17OptionsDialog->Run();
	}
	else
	{
		g_pC17OptionsDialog->SetVisible( false );
	}
};

void C17Options_Destroy()
{
	if( g_pC17OptionsDialog != NULL )
	{
		g_pC17OptionsDialog->DestroySelf();
	}
}

//-----------------------------------------------------------------------------
// CC17OptionsDialog - Constructor
//-----------------------------------------------------------------------------
CC17OptionsDialog::CC17OptionsDialog(vgui::Panel *parent) : PropertyDialog(parent, "OptionsDialog")
{
	vgui::VPANEL pParent = enginevgui->GetPanel( PANEL_GAMEUIDLL );
	SetParent( pParent );
	SetBounds(0, 0, 512, 512);
	SetSizeable( false );
	SetPaintBackgroundEnabled( true );
	SetPaintBorderEnabled( true );
	SetDeleteSelfOnClose( true );

	SetTitle("#GameUI_C17Options", true);

	m_pC17VideoPage = new CC17OptionsSubVideo( this, "VideoPage" );
	m_pC17GameplayPage = new CC17OptionsSubGameplay( this, "GameplayPage" );
	//m_pC17NovintPage = new CC17OptionsSubNovint( this, "NovintPage" );
	m_pC17FaceAPIPage = new CC17OptionsSubFaceAPI( this, "FaceAPIPage" );
	//m_pC17MaxsiPage = new CC17OptionsSubMaxsi( this, "MaxsiPage" );
	AddPage(m_pC17VideoPage, "#GameUI_C17Options_Video");
	AddPage(m_pC17GameplayPage, "#GameUI_C17Options_Gameplay");
	//AddPage(m_pC17NovintPage, "#GameUI_C17Options_Novint");
	AddPage(m_pC17FaceAPIPage, "#GameUI_C17Options_FaceAPI");
	//AddPage(m_pC17MaxsiPage, "#GameUI_C17Options_Maxsi");
	
	SetApplyButtonVisible( true );

	GetPropertySheet()->SetTabWidth(72);
}

//-----------------------------------------------------------------------------
// ~CC17OptionsDialog - Destructor
//-----------------------------------------------------------------------------
CC17OptionsDialog::~CC17OptionsDialog()
{
	SetParent( (vgui::Panel *) NULL );

	if( g_pC17OptionsDialog )
	{
		g_pC17OptionsDialog->MarkForDeletion();
		g_pC17OptionsDialog = NULL;
	}

	// Do we actually need to delete these?
	if( m_pC17VideoPage )
	{
		m_pC17VideoPage->MarkForDeletion();
		m_pC17VideoPage = NULL;
	}

	if( m_pC17GameplayPage )
	{
		m_pC17GameplayPage->MarkForDeletion();
		m_pC17GameplayPage = NULL;
	}

	/*if( m_pC17NovintPage )
	{
		m_pC17NovintPage->MarkForDeletion();
		m_pC17NovintPage = NULL;
	}*/

	if( m_pC17FaceAPIPage )
	{
		m_pC17FaceAPIPage->MarkForDeletion();
		m_pC17FaceAPIPage = NULL;
	}

	/*if( m_pC17MaxsiPage )
	{
		m_pC17MaxsiPage->MarkForDeletion();
		m_pC17MaxsiPage = NULL;
	}*/
}

//-----------------------------------------------------------------------------
// Activate - Activates the dialog & brings it to the front
//-----------------------------------------------------------------------------
void CC17OptionsDialog::DestroySelf()
{
	//Technically, this should be the same thing as FinishClose.
	SetVisible( false );
	MarkForDeletion();
}

//-----------------------------------------------------------------------------
// Activate - Activates the dialog & brings it to the front
//-----------------------------------------------------------------------------
void CC17OptionsDialog::Activate()
{
	BaseClass::Activate();

	EnableApplyButton( true );
}

//-----------------------------------------------------------------------------
// Run: Opens the dialog
//-----------------------------------------------------------------------------
void CC17OptionsDialog::Run()
{
	SetTitle("#GameUI_C17Options", true);
	Activate();
	PositionDialog();

	if( m_pC17VideoPage )
	{
		m_pC17VideoPage->SetComboBoxDefaults();
	}

	if( m_pC17GameplayPage )
	{
		m_pC17GameplayPage->SetComboBoxDefaults();
	}

	/*if( m_pC17NovintPage )
	{
		m_pC17NovintPage->SetComboBoxDefaults();
	}*/

	if( m_pC17FaceAPIPage )
	{
		m_pC17FaceAPIPage->SetComboBoxDefaults();
	}

	/*if( m_pC17MaxsiPage )
	{
		m_pC17MaxsiPage->SetComboBoxDefaults();
	}*/
}

//-----------------------------------------------------------------------------
// OnOK: Fired when the OK button is clicked
//-----------------------------------------------------------------------------
bool CC17OptionsDialog::OnOK( bool applyOnly )
{
	BaseClass::OnOK( true );
	
	//This calls for each of our tabs comboboxes to run their true/false console command.
	if( m_pC17VideoPage )
	{
		m_pC17VideoPage->OnApplyChanges();
	}

	if( m_pC17GameplayPage )
	{
		m_pC17GameplayPage->OnApplyChanges();
	}

	/*if( m_pC17NovintPage )
	{
		m_pC17NovintPage->OnApplyChanges();
	}*/

	if( m_pC17FaceAPIPage )
	{
		m_pC17FaceAPIPage->OnApplyChanges();
	}

	/*if( m_pC17MaxsiPage )
	{
		m_pC17MaxsiPage->OnApplyChanges();
	}*/

	return true;
}

void CC17OptionsDialog::OnCommand(const char *command)
{
	BaseClass::OnCommand( command );

	EnableApplyButton( true );
}

//-----------------------------------------------------------------------------
// OnClose:
//-----------------------------------------------------------------------------
void CC17OptionsDialog::OnClose()
{
	BaseClass::OnClose();
}

//-----------------------------------------------------------------------------
// PositionDialog: Positions the dialog in the center of the screen,
// wmaintaining a correct size
//-----------------------------------------------------------------------------
void CC17OptionsDialog::PositionDialog( void )
{
	int x, y, ww, wt, wide, tall;
	vgui::surface()->GetWorkspaceBounds( x, y, ww, wt );
	g_pC17OptionsDialog->GetSize(wide, tall);

	g_pC17OptionsDialog->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}