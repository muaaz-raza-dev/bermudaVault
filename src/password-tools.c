#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sodium.h>
#include "types.h"
// #include <openssl/sha.h>

int check_password_strength(char *pass){ 
    int i = 0;
    bool is_small_case = false, is_upper_case = false, is_numbers = false, is_special_char = false;
    while (pass[i] != '\0' && pass[i] != '\n'){
        int ascii_value = (int)pass[i];
        if ((ascii_value >= 32 && ascii_value <= 47) || ascii_value == 95 || ascii_value == 96 || ascii_value == 0|| (ascii_value >= 123 && ascii_value <= 126)){
            is_special_char = true;
        }
        else if(ascii_value >= 48 && ascii_value <= 57) is_numbers = true;
        else if (ascii_value >= 65 && ascii_value <= 90) is_upper_case = true;
        else if (ascii_value >= 97 && ascii_value <= 122) is_small_case = true;
        i++;
    }
    int length = i + 1;

    if(length < 8) {
        printf("Password is too short, atleast 8 characters are required. \n");
        return 4;
    }

    if (is_small_case &&is_special_char &&is_upper_case &&is_numbers ){
        printf("Password is strong");
        return 0;
    }
    else if(is_small_case && is_upper_case){
        printf("Password is weak ! ");
        return 1;
    }
    else {
        printf("Password is too weak ! ");
        return 2;
    }

}



int generate_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES]){
    FILE *fptr = fopen("./data/user.dat","wb");
    if (fptr == NULL){
        printf("Not found");
        return 0;
    }
    
    unsigned char salt[SALT_LEN] ;
    unsigned char kek[crypto_secretbox_KEYBYTES];

    randombytes_buf(salt, SALT_LEN);
    crypto_pwhash (kek, sizeof kek, PASSWORD, strlen(PASSWORD), salt,
    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
    crypto_pwhash_ALG_DEFAULT);

    unsigned char dek[crypto_secretbox_KEYBYTES];
    randombytes_buf(dek, sizeof dek);

    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof nonce);

    unsigned char encrypted_dek[crypto_secretbox_MACBYTES + sizeof dek];
    crypto_secretbox_easy(encrypted_dek, dek, sizeof dek, nonce, kek);

    
    memcpy(dek, DEK,crypto_secretbox_KEYBYTES );


    VaultHeader vault ;
    memcpy(vault.salt, salt, sizeof vault.salt);
    memcpy(vault.nonce, nonce, sizeof vault.nonce);
    memcpy(vault.ciphertext, encrypted_dek, sizeof encrypted_dek);
    vault.ciphertext_len = sizeof encrypted_dek;

    fwrite(&vault,sizeof(vault),1,fptr);
    fclose(fptr);

    printf("Logged in successfully !");

    return 0;
}



int load_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES]){
    FILE *fptr = fopen("./data/user.dat","rb");
    VaultHeader vault ;
    fread(&vault,sizeof(VaultHeader),1,fptr);
    unsigned char kek[crypto_secretbox_KEYBYTES];
    crypto_pwhash(kek, sizeof kek, PASSWORD, strlen(PASSWORD), vault.salt, crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
    crypto_pwhash_ALG_DEFAULT);
    if (crypto_secretbox_open_easy(DEK, vault.ciphertext, vault.ciphertext_len, vault.nonce, kek) != 0) {
        printf("Incorrect password \n");
        return -1;
    }
    return 0;
}
