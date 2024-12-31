#include "FolderF.h"

int main() {
	char username[50];
	char password[256];
	int login_attempts = 3; // Maximum number of login attempts

	//connectToDatabase return 0 for true amk
	if (connectToDatabase("FolderHashDB.db")) {
		return 1;
	}

	while (1) {
		if (CheckUser() == 0) {
			printf(BOLD_BLACK_COLOR "Create New User\n" RESET_COLOR);
			printf("\nEnter your username:");
			gets_s(username, sizeof(username));
			printf("Enter your password:");
			gets_s(password, sizeof(password));

			addUser(username, password);
		}
		else {
			printf(BOLD_BLACK_COLOR "User Login\n" RESET_COLOR);
			printf("\nEnter the your user name: ");
			gets_s(username, sizeof(username));
			printf("Enter the your password: ");
			gets_s(password, sizeof(password));

			if (checkPassword(username, password)) {
				char choice[2];
				char folderPath[256];
				printf(GREEN_COLOR "The password is correct.\n" RESET_COLOR);


				printf(BOLD_BLACK_COLOR "1. Lock folder\n");
				printf("2. Unlock folder\n" RESET_COLOR);
				printf("Choose an option (1/2): ");
				gets_s(choice, sizeof(choice));

				switch (choice[0]) {
				case '1': {

					printf(BOLD_BLACK_COLOR"Folder Path :" RESET_COLOR);
					gets_s(folderPath, sizeof(folderPath));
					HideFolder(folderPath);
					break;
				}

				case '2': {
					FolderData folderData = GetFolders(username);
					
					int i = 0;
					int rc = 0;
					if (folderData.haveRow == 1) {
						while (folderData.haveRow == 1) {
							printf("%d. \"%s\"\n", i, (const char*)sqlite3_column_text(folderData.rowData, 1));
							i++;
							rc = sqlite3_step(folderData.rowData);
							if (rc == SQLITE_ROW) {
								folderData.haveRow = 1;
							}
							else {
								folderData.haveRow = 0;
							}
						}
						printf(BOLD_BLACK_COLOR "Select a folder to open: " RESET_COLOR);
						int folderIndex;
						scanf("%d", &folderIndex);
						sqlite3_reset(folderData.rowData);
						rc = sqlite3_step(folderData.rowData);
		
						i = 0;
						while (1) {
							if (i == folderIndex) {
								UnhideFolder(folderData.rowData);
								break;
							}
							else rc = sqlite3_step(folderData.rowData);
									
							i++;
						}

						break;
					}
					
				}


				default:
					printf("Invalid choice.\n");
					break;
				}
				break;
			}
			else {
				if (--login_attempts == 0) {
					fprintf(stderr, RED_COLOR "Maximum entry attempts exceeded.\n" RESET_COLOR);
					break;
				}
				else
					printf(GREEN_COLOR "Remaining right of entry: %d\n" RESET_COLOR, login_attempts);
			}
		}
	}

	// Close database connection
	closeConnection();
	return 0;
}
