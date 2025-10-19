#include <stdio.h>
#include "types.h"

int operation_executer(unsigned char dek[crypto_secretbox_KEYBYTES]) {
    while (1) {
        int operation_num = 0;
        printf("\n\033[1;36m================= Password Vault Menu =================\033[0m\n");
        printf("  \033[1;33m[1]\033[0m  Add a new record\n");
        printf("  \033[1;33m[2]\033[0m  Read saved records\n");
        printf("  \033[1;33m[3]\033[0m  Update an existing record\n");
        printf("  \033[1;33m[4]\033[0m  Delete a record\n");
        printf("  \033[1;33m[5]\033[0m  Change master password\n");
        printf("  \033[1;33m[6]\033[0m  Exit\n");
        printf("\033[1;36m--------------------------------------------------------\033[0m\n");
        printf("\033[1;32mSelect an option (1-6): \033[0m");


        if (scanf("%d", &operation_num) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // clear the input buffer
            while (getchar() != '\n');
            continue;
        }

        switch (operation_num) {
            case 1:
                write_record(dek);
                break;

            case 2:
                read_vault(dek);
                break;

            case 3:
                update_record(dek);
                break;

            case 4:
                delete_record(dek);
                break;

            case 5:
                change_master_password(dek);
                break;

            case 6:
                printf("Exiting vault... Goodbye!\n");
                return 0;

            default:
                printf("Invalid operation selected. Please try again.\n");
                continue;
        }
    }
}
