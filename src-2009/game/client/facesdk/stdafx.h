// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX
#define STDAFX

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here
#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <conio.h>
#include "sm_api.h"

#endif
