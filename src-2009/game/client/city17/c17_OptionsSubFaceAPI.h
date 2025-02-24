//========= Copyright � 2009, Team Gear & Nigredo Studios, All rights reserved. ============//
//
// Purpose: City 17's options dialog, built from NightFall code, originally created by Matt Stafford.
// Implements the FaceAPI Options submenu.
//
//==========================================================================================//

#ifndef C17_OPTIONS_SUB_FACEAPI_H
#define C17_OPTIONS_SUB_FACEAPI_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>

class CCvarComboBox;

namespace vgui
{
    class Label;
    class Panel;
}


class CC17OptionsSubFaceAPI : public vgui::PropertyPage
{
public:
	CC17OptionsSubFaceAPI( vgui::Panel *parent, const char *name );
	~CC17OptionsSubFaceAPI();

	virtual void SetComboBoxDefaults();
	virtual void OnResetData();
	virtual void OnApplyChanges();

	void SetupTooltips( void );

protected:
    virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:

	CCvarComboBox		*m_pFaceBox;

	typedef vgui::PropertyPage BaseClass;
};



#endif // C17_OPTIONS_SUB_FACEAPI_H
