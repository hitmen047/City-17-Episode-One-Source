//========= Copyright � 2009, Team Gear & Nigredo Studios, All rights reserved. ============//
//
// Purpose: City 17's options dialog, built from NightFall code, originally created by Matt Stafford.
//
//==========================================================================================//

#ifndef C17_OPTIONSDIALOG_H
#define C17_OPTIONSDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "c17_OptionsSubVideo.h"
#include "c17_OptionsSubGameplay.h"
//#include "c17_OptionsSubNovint.h"
#include "c17_OptionsSubFaceAPI.h"
//#include "c17_OptionsSubMaxsi.h"
#include <vgui_controls/Frame.h>
#include <vgui_controls/PropertyDialog.h>

namespace vgui
{
class Button;
class CheckButton;
class Label;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CC17OptionsDialog : public vgui::PropertyDialog
{
public:
	CC17OptionsDialog(vgui::Panel *parent);
	~CC17OptionsDialog();

	void DestroySelf();

	void Run();
	virtual void Activate();
	void PositionDialog( void );

private:
	CC17OptionsSubVideo *m_pC17VideoPage;
	CC17OptionsSubGameplay *m_pC17GameplayPage;
	//CC17OptionsSubNovint	*m_pC17NovintPage;
	CC17OptionsSubFaceAPI *m_pC17FaceAPIPage;
	//CC17OptionsSubMaxsi *m_pC17MaxsiPage;

protected:
	virtual bool OnOK( bool applyOnly );
	virtual void OnClose();
	virtual void OnCommand(const char *command);

	typedef vgui::PropertyDialog BaseClass;


};

#endif // C17_OPTIONSDIALOG_H
