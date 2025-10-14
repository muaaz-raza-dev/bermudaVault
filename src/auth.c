#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include <stdbool.h>
#include <sodium.h>
#include "types.h"

typedef struct {
    char password[30];
} master_credentials;

int create_account(char *password,unsigned char DEK[crypto_secretbox_KEYBYTES]);

int authenticate(unsigned char *DEK){
    FILE *fptr = fopen("./data/user.dat","rb");
    master_credentials cred ;
    bool is_first_try = true;
    if(fptr ==NULL){
        printf("------------- Welcome to BermudaVault -------------\n");
        printf("Set the master password and remeber it , to access your saved passwords \n");

        while (1){
            printf("Password : ");
            if (!is_first_try) getchar();
            fgets(cred.password,sizeof(cred.password),stdin);
            is_first_try =false;
            int strength_status = check_password_strength(cred.password); 
            if (strength_status == 4){continue;}
            if(strength_status == 0){ 
                break;
            }
            else{
                int is_change_password = 0;
                printf("\n-Change the password [1] \n-Keep it [0 or anything]\n ");
                scanf("%d",&is_change_password);
                if (is_change_password ==1) continue;
                else break;
            }
        }
        create_account(cred.password,DEK);
    }
    else {
        while (1){
            int decryption_status= 0;
            printf("Password : ");
            if (!is_first_try) getchar();
            fgets(cred.password,sizeof(cred.password),stdin);
            decryption_status = load_dek(cred.password,DEK);
            if (decryption_status == 0){
                break;
            }
            else if (decryption_status ==-1){
            printf("Incorrect password \n");
            }
        }
    }
return 0;
}

int login_n_load_dek(char *password,char DEK[crypto_secretbox_KEYBYTES]){
    int status = load_dek(password,DEK);
    return status;
}

int create_account(char *password,unsigned char DEK[crypto_secretbox_KEYBYTES]){
    generate_dek(password,DEK);
    return 0;
}