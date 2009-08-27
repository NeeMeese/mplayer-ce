/****************************************************************************
 * MPlayer CE
 * Tantric 2009
 *
 * filebrowser.h
 *
 * Generic file routines - reading, writing, browsing
 ****************************************************************************/

#ifndef _FILEBROWSER_H_
#define _FILEBROWSER_H_

#include <unistd.h>
#include <gccore.h>

#define MAXJOLIET 255

typedef struct
{
	char dir[MAXPATHLEN]; // directory path of browserList
	int numEntries; // # of entries in browserList
	int selIndex; // currently selected index of browserList
	int pageIndex; // starting index of browserList page display
	int size; // # of entries browerList has space allocated to store
} BROWSERINFO;

typedef struct
{
	u64 offset; // DVD offset
	unsigned int length; // file length
	time_t mtime; // file modified time
	char isdir; // 0 - file, 1 - directory
	char filename[MAXJOLIET + 1]; // full filename
	char displayname[MAXJOLIET + 1]; // name for browser display
	int icon; // icon to display
} BROWSERENTRY;

enum
{
	ICON_NONE,
	ICON_FOLDER,
	ICON_SD,
	ICON_USB,
	ICON_DVD,
	ICON_SMB
};

extern BROWSERINFO browser;
extern BROWSERENTRY * browserList;

bool IsDeviceRoot(char * path);
int UpdateDirName(int method);
int FileSortCallback(const void *f1, const void *f2);
void ResetBrowser();
bool AddBrowserEntry();
int BrowserChangeFolder(bool updateDir = true);
int BrowseDevice();

#endif
