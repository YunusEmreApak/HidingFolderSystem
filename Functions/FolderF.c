#include <FolderF.h>


char* RandomGUID() {
    char* randomGUID = malloc(37); 

    srand((unsigned int)time(NULL));
    snprintf(randomGUID, 37, "%08X-%04X-%04X-%04X-%04X%08X",
        rand(), rand() % 0xFFFF, rand() % 0xFFFF,
        rand() % 0xFFFF, rand() % 0xFFFF, rand());
    return randomGUID;
}
void GetParentOfPath(char* folderPath) {
    //New folder
    const char* lastSlash = strrchr(folderPath, '\\'); 
    if (!lastSlash) {
        lastSlash = strrchr(folderPath, '/'); 
    }

    if (lastSlash) {
        size_t parentPathLength = lastSlash - folderPath + 1; 
        snprintf(folderPath, parentPathLength + 1, "%s", folderPath); 
    }
}
void GetFolderName(const char* folderPath, char* folderName) {
    size_t len = strlen(folderPath);
    char trimmedPath[256];

    if (len > 0 && (folderPath[len - 1] == '\\' || folderPath[len - 1] == '/')) {
        strncpy(trimmedPath, folderPath, len - 1);
        trimmedPath[len - 1] = '\0';
        folderPath = trimmedPath;
    }

    const char* lastSlash = strrchr(folderPath, '\\');
    if (!lastSlash) {
        lastSlash = strrchr(folderPath, '/');
    }

    if (lastSlash) {
        strcpy(folderName, lastSlash + 1);
    }
    else {
        strcpy(folderName, folderPath);
    }
}
void HideFolder(const char* folderPath) {
    char command[256];
    char* folderGUID = RandomGUID();

    unsigned char encryptedGUID[48];
    size_t encryptedGUIDLen;

    // Encrypt the GUID
    EncryptGUID(folderGUID, encryptedGUID, &encryptedGUIDLen);
    if (encryptedGUIDLen == 0) {
        fprintf(stderr, RED_COLOR "Failed to encrypt GUID.\n" RESET_COLOR);
        return;
    }
    char folderName[256];
    GetFolderName(folderPath, folderName);
    
    stmt = GetUser();
    if (!stmt) {
        fprintf(stderr, RED_COLOR "User information could not be retrieved.\n" RESET_COLOR);
        return;
    }

    const char* username = (const char*)sqlite3_column_text(stmt, 0);
    if (!username) {
        fprintf(stderr, RED_COLOR "No valid user found.\n" RESET_COLOR);
        sqlite3_finalize(stmt);
        return;
    }

    
    if (!AddFolder(folderName, encryptedGUID, username, folderPath)) {
        fprintf(stderr, RED_COLOR "Folder information could not be added to the database.\n" RESET_COLOR);
    }

    //change the name of a folder
    snprintf(command, sizeof(command), "ren \"%s\" \"Control Panel.{%s}\"", folderPath, folderGUID);
    if (system(command) != 0) {
        fprintf(stderr, RED_COLOR "Failed to rename folder to hidden format.\n" RESET_COLOR);
        return;
    }
    GetParentOfPath(folderPath);
    // add hidden and system properties
    snprintf(command, sizeof(command), "attrib +h +s \"%sControl Panel.{%s}\"", folderPath, folderGUID);
    if (system(command) != 0) {
        fprintf(stderr, RED_COLOR "Failed to set hidden attributes for folder.\n" RESET_COLOR);
        return;
    }

    printf(GREEN_COLOR "Folder hidden successfully: %s \n" RESET_COLOR,folderGUID);
}

void UnhideFolder(sqlite3_stmt* stmt) {
    const unsigned char* encryptedGUID = sqlite3_column_text(stmt, 0);
    const char* folderName = sqlite3_column_text(stmt, 1);
    char* originalFolderPath = sqlite3_column_text(stmt, 3);

    if (!encryptedGUID || !folderName || !originalFolderPath) {
        fprintf(stderr, "Failed to retrieve encrypted GUID, folder name, or path from the database.\n");
        return;
    }

    char decryptedGUID[64];
    int encryptedGUIDLength = sqlite3_column_bytes(stmt, 0);

    
    // Decrypt the GUID
    DecryptGUID((unsigned char*)encryptedGUID, decryptedGUID, encryptedGUIDLength);
    if (strlen(decryptedGUID) == 0) {
        fprintf(stderr, RED_COLOR "Failed to decrypt GUID.\n" RESET_COLOR);
        return;
    }

    char command[256];
    GetParentOfPath(originalFolderPath);
    // Remove hidden and system attributes
    snprintf(command, sizeof(command), "attrib -h -s \"%sControl Panel.{%s}\"", originalFolderPath, decryptedGUID);
    if (system(command) != 0) {
        fprintf(stderr, RED_COLOR "Failed to remove attributes from folder: %s\n " RESET_COLOR, decryptedGUID);
        return;
    }

    // Rename the folder back to its original name
    snprintf(command, sizeof(command), "ren \"%sControl Panel.{%s}\" \"%s\"",originalFolderPath , decryptedGUID, folderName);
    if (system(command) != 0) {
        fprintf(stderr, RED_COLOR "Failed to rename folder: %s\n" RESET_COLOR, decryptedGUID);
        return;
    }
    DeleteFolder(folderName);
    printf(GREEN_COLOR "Folder successfully restored to its original path: %s\n" RESET_COLOR, originalFolderPath);
}
