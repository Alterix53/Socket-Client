#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include <stdio.h>
#include <tchar.h>
#include <afxsock.h>

using namespace std;
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _AFXDLL

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by Demo_Client.rc
//

#define IDS_APP_TITLE			103

// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE	101
#define _APS_NEXT_COMMAND_VALUE		40001
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		101
#endif
#endif

#include <SDKDDKVer.h>

// #include <unistd.h>
#include <cstdlib>
#include <signal.h>


#include "ConsoleSetting.h"


struct fileZip {
	string name;
	int size;
	string sizeType;
};

void printServerFile(vector <fileZip> file);
fileZip getFileInfo(string s);
void readInputFile(string filename, vector<fileZip>& f, int alreadyDownload);
int checkIfDownloaded(vector<fileZip> ServerFile, string filename);
void printPercent(long long size, long long currentSize, int coordY);
void DownloadBuffer(string filename, vector<char> data, int size);
void downFile(CSocket& client, string filename, int coordY);

