#include "shared.h"
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>
#include "DatabaseF.h"

char* RandomGUID();
void GetParentOfPath(char* folderPath);
void GetFolderName(const char* folderPath, char* folderName);
void HideFolder(const char* folderPath);
void UnhideFolder(sqlite3_stmt* stmt);
