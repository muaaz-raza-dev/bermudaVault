#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include <stdbool.h>
#include <sodium.h>
#include "types.h"

typedef struct {
    char password[30];
} master_credentials;



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
        generate_dek(cred.password,DEK);
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
            
        }
    }
return 0;
}



int encrypt_vault_password(char plain_text_password[MAX_PASS_LEN],Encrypted_Password *pwd,unsigned char dek[crypto_secretbox_KEYBYTES]){
    int pwd_len = strlen(plain_text_password);
    unsigned long long cipher_len = crypto_secretbox_MACBYTES + (unsigned long long)pwd_len;

    randombytes_buf(pwd->nonce,sizeof (pwd->nonce));
    if (crypto_secretbox_easy(pwd->ciphertext,(const unsigned char *)plain_text_password,pwd_len,pwd->nonce,dek) != 0) {
    printf("Error occured while storing credentials");
    return -1;
    }

    pwd->ciphertext_len = cipher_len;

    return 0;
}



int decrypt_vault_password(char plain_text_password[MAX_PASS_LEN],Encrypted_Password *pwd,unsigned char dek[crypto_secretbox_KEYBYTES]){
    int pwd_len = pwd->ciphertext_len - crypto_secretbox_MACBYTES;

    if (crypto_secretbox_open_easy((unsigned char *)plain_text_password,pwd->ciphertext,pwd->ciphertext_len,pwd->nonce,dek) != 0) {
    printf("Error occured while reading credentials");
    return -1;
    }
    if (pwd_len < MAX_PASS_LEN)
        plain_text_password[pwd_len] = '\0';
    else
        plain_text_password[MAX_PASS_LEN - 1] = '\0';
        plain_text_password[pwd_len] = '\0';    
    return 0;
}