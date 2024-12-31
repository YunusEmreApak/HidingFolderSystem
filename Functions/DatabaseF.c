#include "DatabaseF.h"


void exit_with_error(sqlite3* db, const char* msg) {
	fprintf(stderr, "%s: %s\n", msg, sqlite3_errmsg(db));
	closeConnection();
	exit(1);
}
// Connecting to the database & table seeds
int connectToDatabase(const char* dbName) {
	int rc = sqlite3_open(dbName, &db); // Veritabanýný aç
	if (rc) {
		exit_with_error(db, "Connection error: \n");
		return rc;
	}
	printf(GREEN_COLOR"Connecting to the database: %s\n" RESET_COLOR, dbName);

	// Tabloyu oluþtur
	const char* createTableQuery =
		"CREATE TABLE IF NOT EXISTS UserPasswords ("
		"Username TEXT PRIMARY KEY,"
		"PasswordHash TEXT NOT NULL"
		");";
	rc = sqlite3_exec(db, createTableQuery, 0, 0, 0);

	const char* createFolderDataTableQuery =
		"CREATE TABLE IF NOT EXISTS FolderData ("
		"FolderGUID TEXT PRIMARY KEY,"
		"FolderName TEXT NOT NULL UNIQUE,"
		"Username TEXT NOT NULL,"
		"FolderPath TEXT NOT NULL UNIQUE,"
		"FOREIGN KEY (Username) REFERENCES UserPasswords (Username)"
		");";
	rc = sqlite3_exec(db, createFolderDataTableQuery, 0, 0, 0);

	if (rc != SQLITE_OK) {
		exit_with_error(db, RED_COLOR "Failed to create table: \n" RESET_COLOR);
		return rc;
	}
	printf(GREEN_COLOR "UserPasswords & FolderData table created or already exists.\n" RESET_COLOR);
	return SQLITE_OK;
}

// Baðlantýyý kapatmak
void closeConnection() {
	
	if (db) {
		sqlite3_close(db);
		printf(GREEN_COLOR "The database connection is closed.\n" RESET_COLOR);
	}
}

int addUser(const char* username, const char* password) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	char hashHex[65];

	// Þifreyi hashle
	HashString(password, hash);
	hashToHex(hash, hashHex);

	// Kullanýcýyý veritabanýna ekle
	char query[256];
	sprintf(query, "INSERT INTO UserPasswords (Username, PasswordHash) VALUES ('%s', '%s');", username, hashHex);

	int rc = sqlite3_exec(db, query, 0, 0, 0);
	if (rc != SQLITE_OK) {
		exit_with_error(db, RED_COLOR "Failed to add a new user: \n" RESET_COLOR);
		return 0;
	}

	printf(GREEN_COLOR "New user added: %s\n" RESET_COLOR, username);
	return 1;
}

int CheckUser() {
	stmt = GetUser();
	if (!sqlite3_column_text(stmt, 0)) {
		return 0;
	}

	if (sqlite3_column_text(stmt, 0)) {
		sqlite3_finalize(stmt);
		return 1;
	}
	else {
		sqlite3_finalize(stmt);
		return 0;
	}
}

sqlite3_stmt* GetUser() {
	const char* query = "SELECT * FROM UserPasswords ORDER BY ROWID ASC LIMIT 1;";
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		exit_with_error(db, RED_COLOR "failure fetching data: " RESET_COLOR);
		return NULL;
	}
	sqlite3_step(stmt);
	return stmt;
}


int AddFolder(const char* folderName, const char* folderGUID, const char* username, const char* folderPath) {
	const char* query = "INSERT INTO FolderData (Username, FolderGUID, FolderName, FolderPath) VALUES (?, ?, ?, ?);";

	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		exit_with_error(db, RED_COLOR "SQL query could not be prepared: " RESET_COLOR);
		return 0;
	}

	sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, folderGUID, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, folderName, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, folderPath, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		exit_with_error(db, RED_COLOR "Failed to add folder information: \n " RESET_COLOR);
		return 0;
	}

	sqlite3_finalize(stmt);
	printf(GREEN_COLOR "The folder information has been added to the database: %s (%s)\n" RESET_COLOR, folderName, username);
	return 1;
}

void DeleteFolder(const char* folderName) {
	const char* query = "DELETE FROM FolderData WHERE FolderName = ?";

	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		exit_with_error(db, RED_COLOR "SQL query could not be prepared: " RESET_COLOR);
		return;
	}
	sqlite3_bind_text(stmt, 1, folderName, -1, SQLITE_STATIC);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		exit_with_error(db, RED_COLOR "Failed to delete folder information: \n " RESET_COLOR);
		return;
	}
	sqlite3_finalize(stmt);
	printf(GREEN_COLOR "The folder information has been deleted to the database: %s\n" RESET_COLOR, folderName);
	return;
}

FolderData GetFolders(const char* username) {
	FolderData folderData;
	const char* query = "SELECT * FROM FolderData WHERE Username = ? ;";

	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
	sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
	if (rc != SQLITE_OK) {
		exit_with_error(db, RED_COLOR "failure fetching data: " RESET_COLOR);
		folderData.haveRow = 0;
		folderData.rowData = NULL;
		return folderData;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		folderData.rowData = stmt;
		folderData.haveRow = 1;
	}
	else
	{
		folderData.rowData = NULL;
		folderData.haveRow = 0;
	}
	return folderData;
}

int checkPassword(const char* username, const char* password) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	char hashHex[65];

	// Þifreyi hashle
	HashString(password, hash);
	hashToHex(hash, hashHex);

	const char* query = "SELECT PasswordHash FROM UserPasswords WHERE Username = ?;";
	int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		exit_with_error(db, RED_COLOR "SQL command could not be prepared: " RESET_COLOR);
		return 0;
	}


	sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		const unsigned char* dbHashPtr = sqlite3_column_text(stmt, 0);

		if (dbHashPtr && strcmp(hashHex, (const char*)dbHashPtr) == 0) {
			sqlite3_finalize(stmt);
			return 1; // Þifre doðru
		}
	}

	printf(RED_COLOR "Invalid user or password.\n" RESET_COLOR);
	sqlite3_finalize(stmt);
	return 0;
}


