/****************************************************************************

	COPYRIGHT(C) MAXSI SOFTWARE, JONAS 'SORTIE' TERMANSEN 2008, 2009, 2010

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Maxsi Distribution may not be used for evil. If you intend to use this
		  software for evil, you must purchase a license to use Maxsi Distribution
		  for evil from Maxsi Software.

	THIS SOFTWARE IS PROVIDED BY JONAS 'SORTIE' TERMANSEN ''AS IS'' AND ANY
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL JONAS 'SORTIE' TERMANSEN BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	IN ADDITION, IN NO EVENT SHALL JONAS 'SORTIE' TERMANSEN BE LIABLE IF ANY USERS
	OF THIS SOFTWARE BECOME INFECTED WITH SWINE FLU OR ANY OTHER INFECTIOUS
	DECEASES (INCLUDING, BUT NOT LIMITED TO, BIRD FLU AND ZOMBIE OUTBREAKS), EVEN
	IF ADVISED OF THIS POSSIBILITY. (YES, THIS HAPPENED DURING INTERNAL BETA-TESTING)

	Maxsi Distribution SDK
	An interface to the Maxsi Distribution Platform.

	MaxsiDistributionSDK.cpp
	Code that links to MaxsiDistribution.dll and loads the g_MaxsiDistribution
	interface that can be used by the end product.

****************************************************************************/

#include "cbase.h"

#define MAXSIDISTRIBUTIONSDK_CPP
#include "MaxsiDistributionSDK.h"

/****************************************************************************
	PLEASE CUSTOMIZE THE FOLLOWING
****************************************************************************/

// Enter the name of the folder this mod is within, for instance,
// c:/program files/valve/steam/sourcemods/Altered Transmission/
// should be "Altered Transmission"
#define	MOD_FOLDER_NAME		"c17ep1" // Todo: Is this obsolete?

// Enter the name of this mod as registered on the Maxsi Distribution
// master content server in charge this mod.
#define	MOD_NAME			"City 17: Episode One"

// Maxsi Distribution can automatically detect all content servers by contacting
// a master server that keeps track of everything. Simply register your content
// servers and your product at a working Maxsi Distribution master server and
// every detail about your mod should be automatically downloaded. Enter the url
// (excluding protocol scheme) to the master server you are using
#define MOD_MASTER_SERVER	"sortie.maxsi.dk/distribution/"

// If you would like to present the end users with a feedback form after
// they quit this mod, which allows them to very easily send their thoughts
// on your product back to you, then set this to 1. If not, then set
// this to 0.
#define	MOD_FEEDBACK		0

// If you would like to limit the people allowed to run your mod to a closed
// number of Steam User accounts, set this to 1. This can also be done via
// server-side code on the Content Servers, so this isn't recommended.
// However, if you really want this DRM security layer, feel free to use it
// and enter the exact Steam Usernames of your users below.
// PLEASE NOTE THAT MAXSI DISTRIBUTION IS NOT DRM AND IS NOT DESIGNED TO BE
// RELIABLE OR FOOL-PROOF AND THAT I, JONAS 'SORTIE' TERMANSEN DISCLAIMS ALL
// WARRENTIES WHETHER ANY DRM-LIKE SYSTEMS DESIGNED BY ME WORK OR NOT.
#define MOD_USERNAME_DRM	1

// If you don't want to link to MaxsiDistribution.dll for some reason, set this
// to 1. Otherwise leave it at 0. Please note that g_MaxsiDistribution will be
// NULL if you enable this setting. This option is not supported and may be
// removed in the future.
#define MOD_DONT_LINK		0

// If you do not wish to have automatic updates, simply disable this.
#define MOD_AUTOMATIC_UPDATING 0

// Creates the optional Maxsi Distribution Modules provided in the Maxsi Distribution SDK.
// Feel free to outcomment the interfaces your Maxsi Distribution Powered Application
// does not use. For more information on the interfaces provided in the Maxsi Distribution
// SDK please see http://www.maxsi.dk/developer/index.php?title=Interfaces

void MaxsiDistributionClass::CreateInterfaces()
{
#ifndef MD_MP_SERVER // Some Interfaces are >not< available Server-Side for MP Mods.
	// Maxsi Achievements
	g_MaxsiAchievements		=	(MaxsiAchievementsInterface*)d_MaxsiDistribution.
								GetInterface(MAXSIACHIEVEMENTS_INTERFACE_VERSION);
	g_MaxsiDistribution		->	RegisterModule(g_MaxsiAchievements);
#endif

	return;
}

#if MOD_USERNAME_DRM

	// To use this DRM system, find the usernames of everyone who may run this product.
	// Make sure these usernames are in LOWER CASES. For instance 'Sortie' becomes
	// 'sortie'. Then using a SHA1 Generator, calculate the SHA1 hash of every lower-case username
	// and enter them into the array below. For instance, the Steam Username 'sortie' has the SHA1
	// Hash 092EC58085AE5E368BC39D18752740FF81A2D09C which becomes the entry in the array below
	// SHA1_t(0x092EC580, 0x85AE5E36, 0x8BC39D18, 0x752740FF, 0x81A2D09C ), // Sortie

	SHA1_t	AllowedUsernamesHashes[]	=
	{
		//----------------------
		// Maxsi Distribution
		//----------------------
		SHA1_t(0x092EC580, 0x85AE5E36, 0x8BC39D18, 0x752740FF, 0x81A2D09C), // Sortie

		//----------------------
		// GEAR
		//----------------------
		SHA1_t(0x32F27816, 0x9427C29D, 0xEFCC3489, 0xA4F42BF0, 0x7B290CB9), // MrTwoVideoCards
		SHA1_t(0xCCF863DD, 0x2EFB4616, 0x7CE8F37E, 0x4390C131, 0x583C2E15), // 1/4 Life
		SHA1_t(0x820832BD, 0xC5B77E4C, 0xD47DD56D, 0xF42FE3CE, 0x80EECDAF), // 1/4 Lifetop
		SHA1_t(0x4CF9BD0F, 0x549C2DB7, 0xD3543288, 0x32E66068, 0xE353C1BE), // .cyberkave
		SHA1_t(0xA19F3A36, 0x797FC1CE, 0x3E33D3C9, 0xC690F1F7, 0x9337FBEA), // diwako
		SHA1_t(0x01F9806E, 0xEE1EB26B, 0xF2307A0C, 0xDC2A3190, 0x0D42D5FF), // Sam za Nemesis
		SHA1_t(0xA5C6A4C4, 0x597F9836, 0x5D216602, 0xF74163B1, 0xB7FF206A), // Leo Leonidas
		SHA1_t(0x78726BF5, 0x97ABCCC6, 0x30D04577, 0x0929813B, 0x228677CE), // Mess
		SHA1_t(0xD05D5219, 0x0B344FF2, 0x779A69C4, 0xD4757281, 0xED05097C), // PelPix
		SHA1_t(0x2BA68962, 0xBBF1ECD5, 0x46391294, 0x25FA9996, 0xCC3F302E), // Edli
		SHA1_t(0x98DCB732, 0xDD83EAD4, 0xCFE0D97A, 0x40F1B40D, 0x4BE48174), // Da Big Man
		//SHA1_t(0xD75606A2, 0x155C9EB5, 0x7076445E, 0xDC83B42D, 0xA21C9BD2), // mm3guy - REMOVED
		SHA1_t(0xAF7470EA, 0x7C312589, 0xC694F488, 0x82A947D2, 0x4434C8A1), // VESS
		SHA1_t(0x206F7899, 0xB854971E, 0x1FBE0585, 0x8C71CB95, 0x889765A1), // Captain Capslock
		SHA1_t(0xBDD6A107, 0x9767C405, 0xB18C0ED2, 0x42138C1F, 0x469B2B5D), // fps_noobkiller
		SHA1_t(0x8834BEB1, 0x7ADD35F1, 0x1679CFC5, 0xDADA303F, 0xA6A0E428), // Bites
		SHA1_t(0x5E5F22D5, 0x9C6401F4, 0x727EDD28, 0x58F9DD2B, 0xFF457CF6), // Donald Brent O'Gara
		SHA1_t(0x835E5CEC, 0xEAE0E1AB, 0xC89D987F, 0xB3509E7F, 0x33DC16BE), // The Postman
		SHA1_t(0x665C49B5, 0xA4175C64, 0x18654927, 0xBFC83215, 0x1C7BB636), // jig
		SHA1_t(0xC8A0EB21, 0xA2C40D10, 0xD7C4FD46, 0xC075218A, 0x60C8281D), // Zipfinator
		SHA1_t(0x23A5A4CF, 0xC16A85A7, 0x4765B2C3, 0xF3E004CB, 0x50C9B273), // MaxOfS2D

		//----------------------
		// Testers
		//----------------------
		SHA1_t(0xDC4890C3, 0xB8A89161, 0xAB09548A, 0x4DE141E5, 0x7010A26A), // Planet Phillip
		SHA1_t(0x271DAEAF, 0x8FD8B986, 0x19828097, 0xE00422CC, 0xD2D31E40), // KG
		SHA1_t(0x7B915538, 0x1073BD83, 0x6454F4DC, 0x2D5E0B87, 0x43D7221F), // Mr. Awesome
		SHA1_t(0xAEB5BCA1, 0xBBE5ED82, 0xF514BA02, 0x639D6BE1, 0x0FF52A2F), // No.00 Dylan
		// Wraiyth
		SHA1_t(0xB47ECFA4, 0x8BD8B946, 0xC54ED973, 0x53B8CBD7, 0xFC1E1058), // Mr. Happy
		SHA1_t(0xDEE3E8AE, 0xABC4E3AC, 0x079FB0A7, 0x491927B7, 0xAB57F1D3), // MrBlank88
		SHA1_t(0xC8A0EB21, 0xA2C40D10, 0xD7C4FD46, 0xC075218A, 0x60C8281D), // Zip
		SHA1_t(0x5306DC7B, 0xE1232F5A, 0xB65696CF, 0x082D070C, 0x57F4E777), // SuperToaster
		SHA1_t(0xABD453A3, 0xE31204C1, 0xA8042A07, 0x0C47041C, 0x5B4DB83A), // TomatoSoup

		//----------------------
		// Required
		//----------------------
		SHA1_t(0xDA39A3EE, 0x5E6B4B0D, 0x3255BFEF, 0x95601890, 0xAFD80709), // Don't remove or modify this line.
	};

	char*	AllowedUsernames[]	=
	{
		//----------------------
		// Maxsi Distribution
		//----------------------
		"Sortie", // #Sortie

		//----------------------
		// GEAR
		//----------------------
		"MrTwoVideoCards", // MrTwoVideoCards
		"kyle777777", // 1/4 Life
		"kyle777777_laptop", // 1/4 Lifetop
		"kondakov708", // .cyberkave
		"der_ungenommene_name", // diwako
		"shokerpavlovic", // Sam za Nemesis
		"leothelion21", // Leo Leonidas
		"messiah_4_ever@hotmail.com", // Mess
		"pelpix", // PelPix
		"mastr22193", // Edli
		"gatorboy2005", // Da Big Man
		//"mm3guy", // mm3guy - REMOVED
		"lordvess", // VESS
		"sniper_hunter_42", // Captain Capslock
		"teh_ruzzle", // fps_noobkiller
		"lul45", // Bites
		"d_brent_ogara@comcast.net", // Donald Brent O'Gara
		"thypostman", // The Postman
		"aftfc11", // jig
		"zeppelinrocks105", // Zipfinator
		"Max_of_S2D", // MaxOfS2D

		//----------------------
		// Testers
		//----------------------
		"planetphilliphl2", //Planet Phillip
		"kgwhipp", // KG
		"hunterbrute224", // Mr. Awesome
		"dylanthelbug", // No.00 Dylan
		"dominic1399", // Dominic
		"mr_happycd", // Mr. Happy
		"MrBlank88", // MrBlank88
		"zeppelinrocks105", // Zip
		"treykreis", // SuperToaster
		"tomatosoup", // TomatoSoup
		"Geardev", // Lien
		"varionic", // Varion
		"yarharfiddledeedee", // V

		//----------------------
		// Required
		//----------------------
		"\\LastUserName", // Don't remove or modify this line.
	};

	// PLEASE NOTE THAT THIS SYSTEM MAY NOT BE RELIABLE AND THAT IT CAN
	// EASILY BE REMOVED/CHANGED IF YOUR BINARIES ARE NOT SIGNED. MAXSI DISTRIBTUION
	// DOES NOT PROVIDE ANY CRYPOGRAPHIC FUNCTIONS THAT CAN DETECT IF THE
	// USERNAME LIST HAS BEEN CHANGED. DO NOT RELY ON WHETHER THE DETECTED
	// STEAM USERNAME IS CORRECT NOR IF IT'S ON THE LIST ABOVE, UNLESS YOU HAVE
	// REASON TO BELIEVE THAT THIS LIST HAS NOT BEEN CHANGED AND THAT THE
	// STEAM USERNAME DETECTED IS CORRECT.

#endif

/****************************************************************************
	DON'T EDIT ANYTHING BELOW.
	(Unless you know what you are doing, of course)
****************************************************************************/

#ifndef MD_MP_SERVER // Some ConVars are not used server side in MP mods.
ConVar md_developer	( "md_developer", "0", FCVAR_HIDDEN, "Controls what mode Maxsi Distribution is in. 0 = Default, 1 = Developer (More Verbose).");
#endif

// Should help players get the exact support information.
#define	SupportId(Id) "ErrorID: ME" #Id ", see www.maxsi.dk/support/ for more information."

// A global variable to the Interface. Call MaxsiDistribution::InitializeMaxsiDistribution() to initialize it.
MaxsiDistributionInterface*					g_MaxsiDistribution			=	NULL;
MaxsiAchievementsInterface*					g_MaxsiAchievements			=	NULL;
MaxsiDistributionClass						d_MaxsiDistribution;

	char* MDSeekpaths[] =
	{
		"HKEY_CURRENT_USER\\SOFTWARE\\MaxsiDistribution", // Loads from the registry.
		"../../Maxsi Distribution/", // = %steam%/SourceMods/Maxsi Distribution/
		"", // = The bin folder of the current mod. (Not recommended)
	};
		
	#define	SEEK_PATHS							3

	char* Builds[] =
	{
		"MaxsiDistribution.ini", // Decides what build to use by loading the MaxsiDistribution.ini file
		// looking up the [Global] section to file the PreferredBuildxx (xx=32 for 32-bit md, etc).
		// Please do not remove MaxsiDistribution.ini from here. People should be able to override any
		// settings regarding what distribution they want to use. However, if you prefer that people
		// use your custom distribution of Maxsi Distribution, do add it beneath, and increment
		// BUILDS by one.
		
		//"YourBuildHere/",		// Loads a custom build, if present.
		"Sortie/",				// Loads the official Sortie build.
		"",						// Loads whatever is in the search path folder. (Not recommended)
	};

	#define	BUILDS								3

	char* BinFolders[] =
	{
		"dev" PSIZESTR "/",	// Loads whatever is in the devxx/ folder, if present.
		"bin" PSIZESTR "/", // Loads the official release of the current build of Maxsi Distribution.
		"",		// Loads from the search path folder.
	};

	#define	BINARY_FOLDERS						3

	char* Binaries[] =
	{
		"MaxsiDistribution.dll",
		"MaxsiDistributionUpdate.dll",
		"MaxsiFeedback.exe",
		"MaxsiInstaller.exe",
	};

	// Defined in MaxsiDistribution.h
	//#define	BINARY_MAXSIDISTRIBUTION_DLL			0
	//#define	BINARY_MAXSIDISTRIBUTIONUPDATE_DLL		1
	//#define	BINARY_MAXSIFEEDBACK_EXE				2
	//#define	BINARY_MAXSIINSTALLER_EXE				3

/****************************************************************************
	Common functions from MaxsiEngine.dll
****************************************************************************/

	char* MaxsiDistributionClass::BuildString(unsigned int NumParameters, ...)
	{
		va_list param_pt;

		va_start(param_pt,NumParameters); // Call the setup macro
		
		// First calculate the string length

		size_t FinalString_Len = 0;
		char* TMPString = 0;

		for (unsigned int I = 0 ; I < NumParameters ; I++)
		{
			TMPString = va_arg(param_pt,char*);
			if ( TMPString )
			{
				FinalString_Len+=strlen(TMPString);
			}
		}
		FinalString_Len++;


		// Allocate the required string
		char* FinalString = new char[FinalString_Len+10];
		FinalString[0] = 0;

		va_end(param_pt); // Closing macro
		va_start(param_pt,NumParameters); // Call the setup macro

		for (unsigned int I = 0 ; I < NumParameters ; I++)
		{
			TMPString = va_arg(param_pt,char*);
			if ( TMPString )
			{
				strcat_s(FinalString,FinalString_Len,TMPString);
			}
		}
		
		return FinalString;
	}

	bool MaxsiDistributionClass::str_replace(char* input, char* find, char* replace)
	{
		if (!input||!find||!replace) { return false; }
		size_t input_Len = strlen(input);
		size_t find_Len = strlen(find);
		size_t replace_Len = strlen(replace);

		if (find_Len != replace_Len)
		{
			return false;
		}

		size_t X = 0, Y = 0;

		// Loop through X and check at each X if Y follows, if so return X
		for (X = 0; X < input_Len; X++)
		{
			for (Y = 0; Y < find_Len; Y++)
			{
				if ( input[X+Y] != find[Y] )
				{
					break; // Nope.
				}
			}
			if ( Y == find_Len )
			{
				memcpy(input+X,replace,find_Len);
			}
		}
		return true; // Done
	}

	char*	MaxsiDistributionClass::GetWorkingDir()
	{
		char*	CurrentDirectory		=	0;
		char*	CurrentDirectory_TMP	=	_getcwd(NULL,0);

		size_t CurrentDirectory_TMP_Len = strlen(CurrentDirectory_TMP);

		if (CurrentDirectory_TMP[CurrentDirectory_TMP_Len-1] == '\\' ||
			CurrentDirectory_TMP[CurrentDirectory_TMP_Len-1] == '/')
		{
			CurrentDirectory = new char[CurrentDirectory_TMP_Len+1];
			strcpy_s(CurrentDirectory,CurrentDirectory_TMP_Len+1,CurrentDirectory_TMP);
		}
		else
		{
			CurrentDirectory = new char[CurrentDirectory_TMP_Len+2];
			strcpy_s(CurrentDirectory,CurrentDirectory_TMP_Len+1,CurrentDirectory_TMP);
			memset(CurrentDirectory+CurrentDirectory_TMP_Len,'/',1);
			memset(CurrentDirectory+CurrentDirectory_TMP_Len+1,0,1);
		}

		free(CurrentDirectory_TMP);

		return CurrentDirectory;
	}

/****************************************************************************
	Purpose: An easy method to report any errors to the users.
****************************************************************************/
	bool	MaxsiDistributionClass::ReportError(const char* Error, bool DontShowNotepad)
	{
		IngameError(Error);
		int		FileHandle	=	0;			
		char*	WorkingDir	=	GetWorkingDir();
		char*	File		=	BuildString(2,WorkingDir,"MaxsiDistributionError.txt");

		_sopen_s(&FileHandle,File, _O_CREAT | O_TRUNC | O_BINARY | O_WRONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);

		if (FileHandle == -1)
		{
			delete[] WorkingDir;
			delete[] File;
			return false;
		}
		else
		{
			char* Footer =
				"\r\n\r\n"
				"This mod is using Maxsi Distribution Features.\r\n"
				"Unfortunately we couldn't initialize Maxsi Distribution.\r\n"
				"Please make sure you have installed Maxsi Distribution properly.\r\n"
				"For more information please see http://www.maxsi.dk/distribution/\r\n"
				"\r\n"
				"Please contact the developers of this mod for more information.\r\n"
				"Please include the error message above.";

			_write(FileHandle,Error,strlen(Error));
			_write(FileHandle,Footer,strlen(Footer));
			_close(FileHandle);		
		}
		
		char*	ReadMeCommand	=	BuildString(3,"notepad \"", File, "\"");

		if ( !DontShowNotepad )
		{
			WinExec(ReadMeCommand,SW_SHOW);
		}
			
		delete[] ReadMeCommand;
		delete[] WorkingDir;
		delete[] File;
		
		return true;
	}

	// Quick console error reporting
	bool MaxsiDistributionClass::IngameError(const char* Error)
	{
		Color ConsoleColor(255,127,63,255);
		ConColorMsg(ConsoleColor,"Maxsi Distribution Error: ");
		ConColorMsg(ConsoleColor,Error);
		ConColorMsg(ConsoleColor,"\n");
		return true;
	}

	// Quick console information
	bool MaxsiDistributionClass::IngameInfo(const char* Error)
	{
		Color ConsoleColor(255,127,63,255);
		ConColorMsg(ConsoleColor,"Maxsi Distribution Info: ");
		ConColorMsg(ConsoleColor,Error);
		ConColorMsg(ConsoleColor,"\n");
		return true;
	}

	//=============================================================================
	//	bool	IngameMsg(char* Msg);
	//
	//	Writes Msg to the console.
	//=============================================================================

	bool	MaxsiDistributionClass::IngameMsg(const char* Msg)
	{
		Color ConsoleColor(255,127,63,255);
		ConColorMsg(ConsoleColor,Msg);
		return true;
	}

	//=============================================================================
	//	bool	IngameMsgDebug(char* Msg);
	//
	//	Writes Msg to the console.
	//=============================================================================

	bool	MaxsiDistributionClass::IngameMsgDebug(const char* Msg)
	{
#ifndef MD_MP_SERVER
		if ( md_developer.GetBool() == false ) { return false; }
#endif
		Color ConsoleColor(255,127,63,255);
		ConColorMsg(ConsoleColor,Msg);
		return true;
	}


	char*	MaxsiDistributionClass::GetSteamUsername()
	{
		char*	WorkingDir		=	GetWorkingDir();
		str_replace(WorkingDir,"\\","/");
		size_t	WorkingDirLen	=	strlen(WorkingDir);
		size_t	UsernameBegin	=	0;
		size_t	UsernameEnd		=	0;
		size_t	UsernameLen		=	0;
		char*	Username		=	0;
		int		NumSlashes		=	0;

		for (size_t N = WorkingDirLen; N > 0; N--)
		{
			if (WorkingDir[N] == '/'||
				WorkingDir[N] == '\\')
			{
				NumSlashes++;
				if ( NumSlashes == 1)
				{
				}
				else if ( NumSlashes == 2)
				{
					UsernameEnd				=	N-1;
				}
				else if ( NumSlashes == 3)
				{
					// Gather the username
					UsernameBegin			=	N+1;
					UsernameLen				=	UsernameEnd-UsernameBegin+1;
					Username				=	new char[UsernameLen+1];

					memcpy(Username,WorkingDir+UsernameBegin,UsernameLen);

					Username[UsernameLen]	=	0;
				}
			}
		}
		return Username;
	}

	char* MaxsiDistributionClass::GetRegMaxsiRoot( void )
	{
		char*	MDRoot = NULL;
		char	lszValue[MAX_PATH];

		HKEY	hKey;
		LONG	returnStatus;

		size_t	dwType	=	REG_SZ;
		size_t	dwSize	=	MAX_PATH;

		returnStatus = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\MaxsiDistribution\\", 0L,  KEY_READ, &hKey);

		if (returnStatus == ERROR_SUCCESS)
		{
			returnStatus = RegQueryValueExA(hKey, "Root", NULL, (LPDWORD)&dwType,(LPBYTE)&lszValue, (LPDWORD)&dwSize);

			if (returnStatus == ERROR_SUCCESS)
			{
				MDRoot = new char[dwSize+2];
				MDRoot[dwSize] = 0; // Lets null-terminate!
				memcpy(MDRoot,lszValue,dwSize);
				if (MDRoot[dwSize-2] != '/' &&
					MDRoot[dwSize-2] != '\\' )
				{
					MDRoot[dwSize-1]		= '\\';
					MDRoot[dwSize+0]		= 0;
				}
			}

			RegCloseKey(hKey);	
		}

		return MDRoot;
	}

	// 32 bit signed integer, base 10
	char*	MaxsiDistributionClass::IToA(int	In)
	{
		char*	Result	=	new char[12]; // ceiling(32*ln(2)/ln(10)) + room for a leading '-' + null
		_itoa_s(In,Result,12,10);
		return	Result;
	}

	// TODO: There is no function called _uitoa_s!!! Simply pass it on to the 64-bit version
	// 32 bit unsigned integer, base 10
	char*	MaxsiDistributionClass::UIToA(unsigned int	In)
	{
		return UIToA64((unsigned __int64)In);
		//char*	Result	=	new char[11]; // ceiling(32*ln(2)/ln(10)) + null
		//_uitoa_s(In,Result,11,10);
		//return	Result;
	}
	
	// 64 bit signed integer, base 10
	char*	MaxsiDistributionClass::IToA64(__int64	In)
	{
		char*	Result	=	new char[22]; // ceiling(64*ln(2)/ln(10)) + room for a leading '-' + null
		_i64toa_s(In,Result,22,10);
		return	Result;
	}

	// 64 bit unsigned integer, base 10
	char*	MaxsiDistributionClass::UIToA64(unsigned __int64	In)
	{
		char*	Result	=	new char[21]; // ceiling(64*ln(2)/ln(10)) + null
		_ui64toa_s(In,Result,21,10);
		return	Result;
	}

	//=============================================================================
	//	SHA1_t	CalculateSHA1(char* Buffer, size_t BufferLen);
	//
	//	Calculates a 160-bit SHA1 hash
	//=============================================================================

	SHA1_t	MaxsiDistributionClass::CalculateSHA1(char* Buffer, size_t BufferLen)
	{
		// TODO: >NOT< implemented yet!
		return SHA1_t(0x01234567,0x89ABCDEF,0xFEDCBA98,0x76543210,0x01234567);
	}

	//=============================================================================
	//	MaxsiDistributionClass::d_MaxsiDistribution()
	//	MaxsiDistributionClass::~d_MaxsiDistribution()
	//
	//	Makes sure the MaxsiDistribution.dll is unloaded when the game quits.
	//	Oh! And variables are initialized here too!
	//=============================================================================

	MaxsiDistributionClass::MaxsiDistributionClass()
	{
		MaxsiDistributionLibrary	=	NULL;
		iSeekPath					=	0;
		iBuild				=	0;
		iBinFolder					=	0;
		iBinary						=	BINARY_MAXSIDISTRIBUTION_DLL;
	}

	MaxsiDistributionClass::~MaxsiDistributionClass()
	{			
		if ( MaxsiDistributionLibrary ) 
		{
			FreeLibrary(MaxsiDistributionLibrary);
			MaxsiDistributionLibrary	=	NULL;
		}

#ifdef CLIENT_DLL
#if MOD_FEEDBACK

		LPSTARTUPINFOA si			=	new _STARTUPINFOA;
		LPPROCESS_INFORMATION pi	=	new _PROCESS_INFORMATION;

		ZeroMemory( si, sizeof(_STARTUPINFOA) );
		si->cb = sizeof(si);
		ZeroMemory( pi, sizeof(_PROCESS_INFORMATION) );

#ifdef GAME_DLL
		char* GameDir = new char[1024];
		engine->GetGameDir((char*)GameDir, 1024);
#else
		char* GameDir = (char*)engine->GetGameDirectory();
#endif

		bool	Success		=	false;

		for ( size_t I = 0; I < BINARY_FOLDERS; I++ )
		{
			char* TheCommand = BuildString(8,"\"",GameDir,"/bin/",MDSeekpaths[iSeekPath],BinFolders[iBinFolder],Binaries[BINARY_MAXSIFEEDBACK_EXE],"\" \"",GameDir,"\"");

			if( CreateProcessA( NULL,   // No module name (use command line)
				TheCommand,        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				si,            // Pointer to STARTUPINFO structure
				pi )           // Pointer to PROCESS_INFORMATION structure
			)
			{ 
				Success		=	true;

				CloseHandle( pi->hProcess );
				CloseHandle( pi->hThread );
				delete[] TheCommand;
				break;
			}

			delete[] TheCommand;
		}

		if (!Success)
		{
			char* Error = BuildString(1,"LaunchFeedbackApplication::~LaunchFeedbackApplication(), CreateProcessA() Failed! " SupportId(37));
			ReportError(Error);
			delete[] Error;
		}

#ifdef GAME_DLL
		delete[] GameDir;
#endif
		delete pi;
		delete si;
#endif
#endif

	}

	//=============================================================================
	//	bool	InitializeMaxsiDistribution();
	//
	//	Links to MaxsiDistribution.dll and sets g_MaxsiDistribution. If an error,
	//	occurs the function returns false and the game quits and an error message
	//	is presented. If true, the game can safely continue.
	//=============================================================================

	bool	MaxsiDistributionClass::InitializeMaxsiDistribution()
	{
		// Maxsi Distribution.

		int		iBinary		=	BINARY_MAXSIDISTRIBUTION_DLL;

		// Whether we need to try again.
		bool	Relink		=	true;
		int		Result		=	RESULT_DONT_RUN;

		while	(Relink)
		{
			if ( d_MaxsiDistribution.InitializeMaxsiDistributionLibrary(iBinary) )
			{
#if MOD_DONT_LINK
				return true;
#endif
#if defined(CLIENT_DLL) && MOD_AUTOMATIC_UPDATING
				Result	=	g_MaxsiDistribution->UpdateProduct();
#else
				Result	=	RESULT_RUN;
#endif

				if ( Result == RESULT_DONT_RUN )
				{
					Relink	=	false;
				}
				else if ( Result == RESULT_RUN )
				{
					Relink	=	false;
				}
				else if ( Result == RESULT_UPDATE_UPDATING_SOFTWARE )
				{
					iBinary	=	BINARY_MAXSIDISTRIBUTIONUPDATE_DLL;
					Relink	=	true;
				}
				else if ( Result == RESULT_UPDATE_UPDATED_SOFTWARE )
				{
					iBinary	=	BINARY_MAXSIDISTRIBUTION_DLL;
					Relink	=	true;
				}
			}
			else
			{
				Relink		=	false;
			}
		}

		if ( Result == RESULT_RUN )
		{
#ifdef GAME_DLL
			g_pGameSaveRestoreBlockSet->AddBlockHandler( GetMaxsiDistributionSaveRestoreBlockHandler() );
			// TODO: Call g_pGameSaveRestoreBlockSet->RemoveBlockHandler just
			// before CServerGameDLL::DLLShutdown()!
#endif
			
			// Create the g_Maxsi* Interfaces needed by this application.
			CreateInterfaces();

			g_MaxsiDistribution->ReportCrashes();
			//g_MaxsiDistribution->Statistic("Progress","Launched");

			return true;
		}
		else
		{
			//PostQuitMessage(0);

			// Todo: Is this dangerous?
			ExitProcess(0);

			return false;
		}
	}

	//=============================================================================
	//	bool	InitializeMaxsiDistributionLibrary(int Binary);
	//
	//	Links to MaxsiDistribution.dll and sets g_MaxsiDistribution. If an error,
	//	occurs the function returns false and the game quits and an error message
	//	is presented. If true, the game can safely continue.
	//=============================================================================

	bool	MaxsiDistributionClass::InitializeMaxsiDistributionLibrary(int Binary)
	{
		if ( MaxsiDistributionLibrary ) 
		{
			FreeLibrary(MaxsiDistributionLibrary);
			MaxsiDistributionLibrary	=	NULL;
		}

		// Reinitialize variables - in case we are not called for the first time!

		MaxsiDistributionLibrary	=	NULL;
		iSeekPath					=	0;
		iBuild						=	0;
		iBinFolder					=	0;
		iBinary						=	Binary;

		bool	Result			=	false;
		char*	GameDir			=	0;

		char*	Username		=	GetSteamUsername();

#if MOD_USERNAME_DRM

		size_t	UsernameCounter	=	0;
		bool	Found			=	false;

		while ( _stricmp(AllowedUsernames[UsernameCounter],"\\LastUserName") != 0 )
		{
			if ( _stricmp(AllowedUsernames[UsernameCounter],Username) == 0 )
			{
				Found			=	true;
				break;
			}
			UsernameCounter++;
		}

		if ( !Found )
		{
			ReportError(
				"Yo dawg I herd u liek leaks so I call a piper to pipe your pipe "
				"- Gear Team"
				);

			delete[]	Username;
			return false; // Do not launch this mod.
		}

#endif
		
#if MOD_DONT_LINK

		delete[]	Username;

		return	true;

#endif

		if (!engine)
		{
			ReportError("Maxsi Distribution Error: The in-game variable 'engine' isn't set! " SupportId(2));
			return false;
		}

#ifdef GAME_DLL
		GameDir = new char[1024];
		engine->GetGameDir((char*)GameDir, 1024);
#else
		GameDir = (char*)engine->GetGameDirectory();

		if (!GameDir)
		{		
			ReportError("Maxsi Distribution Error: The in-game variable 'engine->GetGameDirectory()'  isn't set! " SupportId(1));
			return false;
		}
#endif

		// Initialize variables.

		char*	BinFolder		=	BuildString(2,GameDir,"/bin/");
		char*	ProductRoot		=	BuildString(2,GameDir,"/../");
		char*	WorkingDir		=	GetWorkingDir();
		char*	Path			=	0;
		char*	MD_INI			=	NULL;
		char*	MD_SEEK			=	NULL;
		bool	FullSearchPath	=	false;

		// Go through all the search paths in order and find the best binary.
		for ( iSeekPath = 0; iSeekPath < SEEK_PATHS; iSeekPath++ )
		{
			// Load from where Maxsi Distribution is installed according to the registry.
			if ( _stricmp(MDSeekpaths[iSeekPath], "HKEY_CURRENT_USER\\SOFTWARE\\MaxsiDistribution") == 0 )
			{
				char*	PreferredRoot		=	GetRegMaxsiRoot();
				if ( PreferredRoot )
				{
					MD_SEEK					=	MDSeekpaths[iSeekPath];
					MDSeekpaths[iSeekPath]	=	PreferredRoot;
					FullSearchPath			=	true;
				}
				else
				{
					continue;
				}
			}

			for ( iBuild = 0; iBuild < BUILDS; iBuild++ )
			{
				// Load the preferred build from MaxsiDistribution.ini
				if ( _stricmp(Builds[iBuild],"MaxsiDistribution.ini") == 0 )
				{
					char*	Dest		=	new char[MAX_PATH+1];
					char*	MD_INI_FILE =	(FullSearchPath)
							? BuildString(2,MDSeekpaths[iSeekPath],"MaxsiDistribution.ini")
							: BuildString(3,BinFolder,MDSeekpaths[iSeekPath],"MaxsiDistribution.ini");

					str_replace(MD_INI_FILE,"/","\\"); // Silly Windows uses back-slashes

					DWORD	Result	=	GetPrivateProfileStringA(
						"Global",
						"PreferredBuild" PSIZESTR,
						NULL,
						Dest,
						MAX_PATH+1,
						MD_INI_FILE);

					delete[] MD_INI_FILE;
					
					if ( Result == 0 ) // Key/Section/File not found!
					{
						continue;
					}
					else // Found!
					{
						strcat_s(Dest,MAX_PATH+1,"\\");
						MD_INI = Builds[iBuild];
						Builds[iBuild]	=	Dest;
					}
				}

				for ( iBinFolder = 0; iBinFolder < BINARY_FOLDERS; iBinFolder++ )
				{
					char*	Path				=	(FullSearchPath)
							? BuildString(4,MDSeekpaths[iSeekPath],Builds[iBuild],BinFolders[iBinFolder],Binaries[iBinary])
							: BuildString(5,BinFolder,MDSeekpaths[iSeekPath],Builds[iBuild],BinFolders[iBinFolder],Binaries[iBinary]);
					str_replace(Path,"/","\\");

					char*	NewWorkingDir		=	(FullSearchPath)
							? BuildString(3,MDSeekpaths[iSeekPath],Builds[iBuild],BinFolders[iBinFolder])
							: BuildString(4,BinFolder,MDSeekpaths[iSeekPath],Builds[iBuild],BinFolders[iBinFolder]);
					str_replace(NewWorkingDir,"/","\\");
					
					// Change the working directory
					if ( _chdir(NewWorkingDir) != 0)
					{
						// We could not change the working dir, possibly because it does not exist.
						delete[]	NewWorkingDir;
						continue;
					}

					delete[]		NewWorkingDir;

					MaxsiDistributionLibrary	=	LoadLibraryA(Path);
					
					if ( MaxsiDistributionLibrary )
					{
						break;
					}
					else
					{
						delete[] Path;	Path = NULL;
					}
				}

				if ( MD_INI ) // INI file was loaded, clean up memory!
				{
					delete[] Builds[iBuild];
					Builds[iBuild]		=	MD_INI;
					MD_INI				=	NULL;
				}

				if ( MaxsiDistributionLibrary )	{ break; }
			}

			if ( MD_SEEK )
			{
				delete[] MDSeekpaths[iSeekPath];
				MDSeekpaths[iSeekPath]	=	MD_SEEK;
				MD_SEEK					=	NULL;
				if ( !MaxsiDistributionLibrary )
				{
					FullSearchPath		=	false;
				}
			}

			if ( MaxsiDistributionLibrary )	{ break; }
		}		

		if ( MaxsiDistributionLibrary )
		{
			typedef void* (__cdecl* type_GetInterface  ) (char* Version);
			GetInterface = reinterpret_cast<type_GetInterface>(GetProcAddress(MaxsiDistributionLibrary, "GetInterface" ));

			if ( GetInterface )
			{
				g_MaxsiDistribution		=	(MaxsiDistributionInterface*)GetInterface(MAXSIDISTRIBUTION_INTERFACE_VERSION);

				if ( g_MaxsiDistribution == NULL )
				{
					char*		Error		=		BuildString(3,"Maxsi Distribution Error:\r\n\r\nCouldn't Create Interface Version " MAXSIDISTRIBUTION_INTERFACE_VERSION " in ",Path, SupportId(5));
					ReportError(Error);
					delete[]	Error;
				}
				else
				{
					Result	=	true;

					// Initialize Maxsi Distribution, if it haven't already been done.
					if ( g_MaxsiDistribution->IsInitialized() == false)
					{					
						char*	MDRoot			=	(FullSearchPath)
								? BuildString(1,MDSeekpaths[iSeekPath])
								: BuildString(2,BinFolder,MDSeekpaths[iSeekPath]);

						char*	ProductPID		=	UIToA((unsigned int)GetCurrentProcessId());

						// Provide Maxsi Distribution with as much information as possible - in the
						// case that it might need it in the future - right now some of it isn't needed
						// in the stock Maxsi Distribution build. This makes us forward compatible!

						char*	Variables[]		=
						{
							"ContentType",			"Steam/SourceMod", // 1
							"MDPath",				Path, // 2
							"MDRoot",				MDRoot, // 3
							"MDSeekPath",			MDSeekpaths[iSeekPath], // 4
							"MDDistribution",		Builds[iBuild], // 5 // Obsolete: Please delete.
							"MDBuild",				Builds[iBuild], // 6
							"MDBinFolder",			BinFolders[iBinFolder], // 7
							"MDBinary",				Binaries[iBinary], // 8
							"ProductBin",			BinFolder, // 9
							"ProductDirectory",		GameDir, // 10
							"ProductMasterServer",	MOD_MASTER_SERVER, // 11
							"ProductName",			MOD_NAME, // 12
							"ProductPID",			ProductPID, // 13
							"ProductRestartCmd",	GetCommandLineA(), // 14
							"ProductRoot",			ProductRoot, // 15
							"Username",				Username, // 16
							"WorkingDir",			WorkingDir, // 17

							"SDKCallbackVersion",	MAXSIDISTRIBUTION_CLASS_VERSION, // 18
							"SDKCallback",			(char*)&d_MaxsiDistribution, // 19
						};

						g_MaxsiDistribution->Initialize(Variables,19);
					
						delete[]	MDRoot;
					}
				}
			}
			else
			{			
				ReportError("Maxsi Distribution Error:\r\n\r\nCouldn't find function GetInterface in MaxsiDistribution.dll. " SupportId(6));
			}

			// Restore the working directory
			_chdir(WorkingDir);
		}
		else
		{
			LPTSTR pszMessage;
			DWORD dwLastError = GetLastError(); 
			
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwLastError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&pszMessage,
				0, NULL );

			// Display the error message and exit the process
			char*	Error = BuildString(2,
				MOD_NAME " can't be run without Maxsi Distribution installed. "
				"Maxsi Distribution was not detected on your system. Please install "
				"Maxsi Distribution from http://www.maxsi.dk/distribution/\r\n\r\n"
				"Maxsi Distribution is an open-source next-generation mod distribution platform "
				"offering advanced features such as automatic updating, achievements, "
				"statistics, crash reporting, and much more.\r\n\r\nError: ", pszMessage);

			ReportError(Error);

			LocalFree(pszMessage);
		}

		if ( Path) { delete[] Path; }

		delete[] ProductRoot;
		delete[] BinFolder;
		//delete[] LibraryDLL;
		delete[] WorkingDir;
		delete[] Username;
#ifdef GAME_DLL
		delete[] GameDir;
#endif

		return Result;
	}

// To save Maxsi Distribution Session Data in Save Games!
#ifdef GAME_DLL

	// Increment this if the Maxsi Distribution Block Data Format changes.
	// The current format is:
	// size_t DataLength;
	// char Data[DataLength];

	static short MAXSIDISTRIBUTION_SAVE_RESTORE_VERSION = 1;

	//-----------------------------------------------------------------------------

	class CMaxsiDistributionSaveRestoreBlockHandler : public CDefSaveRestoreBlockHandler
	{
	public:
		const char *GetBlockName()
		{
			return "MaxsiDistribution";
		}

		//---------------------------------

		void Save( ISave *pSave )
		{
			pSave->StartBlock( "MaxsiDistribution" );

			char*	Data;
			size_t	DataLength;

			g_MaxsiDistribution->SaveSession(&Data, &DataLength);
			
			pSave->WriteData( (const char*)&DataLength, sizeof(DataLength) );
			pSave->WriteData( (const char*)Data, (int)DataLength );

			g_MaxsiDistribution->SavedSession(Data, DataLength);

			pSave->EndBlock();
		}

		//---------------------------------

		void WriteSaveHeaders( ISave *pSave )
		{
			pSave->WriteShort( &MAXSIDISTRIBUTION_SAVE_RESTORE_VERSION );
		}
		
		//---------------------------------

		void ReadRestoreHeaders( IRestore *pRestore )
		{
			// No reason why any future version shouldn't try to retain backward compatability. The default here is to not do so.
			short version;
			pRestore->ReadShort( &version );
			// only load if version matches and if we are loading a game, not a transition
			m_fDoLoad = ( ( version == MAXSIDISTRIBUTION_SAVE_RESTORE_VERSION ) && 
				( ( MapLoad_LoadGame == gpGlobals->eLoadType ) || ( MapLoad_NewGame == gpGlobals->eLoadType )  ) 
			);
		}

		//---------------------------------

		void Restore( IRestore *pRestore, bool createPlayers )
		{
			if ( m_fDoLoad )
			{
				pRestore->StartBlock();

				size_t	DataLength;

				pRestore->ReadData((char*)&DataLength,sizeof(size_t),0);

				char*	Data	=	new char[DataLength];

				pRestore->ReadData((char*)Data,(int)DataLength,DataLength);

				g_MaxsiDistribution->RestoreSession(Data, DataLength);

				delete[] Data;
				
				pRestore->EndBlock();
			}
		}

	private:
		bool m_fDoLoad;
	};

	//-----------------------------------------------------------------------------

	CMaxsiDistributionSaveRestoreBlockHandler g_MaxsiDistributionSaveRestoreBlockHandler;

	//-------------------------------------

	ISaveRestoreBlockHandler *GetMaxsiDistributionSaveRestoreBlockHandler()
	{
		return &g_MaxsiDistributionSaveRestoreBlockHandler;
	}

#endif