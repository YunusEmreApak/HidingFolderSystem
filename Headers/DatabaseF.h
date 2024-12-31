#include "shared.h"
#include "CryptF.h"
#include <sqlite3.h>

sqlite3* db; // SQLite veritabaný nesnesi
sqlite3_stmt* stmt; // SQL ifadesi nesnesi
typedef struct {
	sqlite3_stmt* rowData;
	int haveRow;
}FolderData;

void exit_with_error(sqlite3* db, const char* msg);
int connectToDatabase(const char* dbName);
void closeConnection();
int addUser(const char* username, const char* password);
int CheckUser();
sqlite3_stmt* GetUser();
int AddFolder(const char* folderName, const char* folderGUID, char* username, char* folderPath);
void DeleteFolder(const char* folderName);
FolderData GetFolders(const char* username);
int checkPassword(const char* username, const char* password);

