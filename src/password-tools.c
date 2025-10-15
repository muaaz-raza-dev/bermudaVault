#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sodium.h>
#include "types.h"
#include <time.h>



int check_password_strength(char *pass){ 
    // 0 success , 4 too short 3 
    int i = 0;
    
    int includes_index = 0;
    while (pass[i] != '\0' && pass[i] != '\n'){
        int ascii_value = (int)pass[i];
        if ((ascii_value >= 32 && ascii_value <= 47) || ascii_value == 95 || ascii_value == 96 || ascii_value == 0|| (ascii_value >= 123 && ascii_value <= 126)){
            includes_index++;
        }
        else if(ascii_value >= 48 && ascii_value <= 57){ includes_index++;}
        else if (ascii_value >= 65 && ascii_value <= 90){ includes_index++;}
        else if (ascii_value >= 97 && ascii_value <= 122){includes_index++;}
        i++;
    }
    int length = i + 1;

    if(length < 8) {
        printf("Password is too short, atleast 8 characters are required. \n");
        return 4;
    }

    if (includes_index ==4){
        printf("Password is strong");
        return 0;
    }
    else if(includes_index ==3){
        printf("Password is just good ! ");
        return 1;
    }
    else if(includes_index ==2){
        printf("Password is weak ! ");
        return 2;
    }
    else {
        printf("Password is too weak ! ");
        return 3;
    }

}



int generate_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES]){
    mkdir("./data", 0700);
    FILE *fptr = fopen("./data/user.dat","wb");
    

    if (fptr == NULL){
        printf("Not found");
        return 0;
    }
    
    unsigned char salt[SALT_LEN] ;
    unsigned char kek[crypto_secretbox_KEYBYTES];

    randombytes_buf(salt, SALT_LEN);
    crypto_pwhash (kek, sizeof(kek), PASSWORD, strlen(PASSWORD), salt,
    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
    crypto_pwhash_ALG_DEFAULT);

    unsigned char dek[crypto_secretbox_KEYBYTES];
    randombytes_buf(dek, sizeof(dek));

    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    unsigned char encrypted_dek[crypto_secretbox_MACBYTES + sizeof dek];
    if( crypto_secretbox_easy(encrypted_dek, dek, sizeof(dek), nonce, kek) !=0){
        printf("Failed to encrypt DEK");
    }


    
    memcpy(DEK, dek,crypto_secretbox_KEYBYTES );


    VaultHeader vault ;
    memcpy(vault.salt, salt, sizeof(vault.salt));
    memcpy(vault.nonce, nonce, sizeof( vault.nonce));
    memcpy(vault.ciphertext, encrypted_dek, sizeof(encrypted_dek));
    vault.ciphertext_len = sizeof(encrypted_dek);
    fwrite(&vault,sizeof(vault),1,fptr);
    fclose(fptr);
    
    
    sodium_memzero(kek, sizeof(kek));

    printf("Setup completed !\n");

    return 0;
}



int load_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES]){
    FILE *fptr = fopen("./data/user.dat","rb");
    
    VaultHeader vault ;
    fread(&vault,sizeof(VaultHeader),1,fptr);
    unsigned char kek[crypto_secretbox_KEYBYTES];
    
    if(crypto_pwhash(kek, sizeof(kek), PASSWORD, strlen(PASSWORD), vault.salt,
                  crypto_pwhash_OPSLIMIT_INTERACTIVE,
                  crypto_pwhash_MEMLIMIT_INTERACTIVE,
                  crypto_pwhash_ALG_DEFAULT) != 0 
        ){
                      printf("Error: Not enough memory for password hash.\n");
        }
    
    ;
    if (crypto_secretbox_open_easy(DEK, vault.ciphertext, vault.ciphertext_len, vault.nonce, kek) != 0) {
        printf("Incorrect password \n");
        return -1;
    }
    return 0;
}

int random_password_gen(char *password,int password_len){
    srand(time(NULL));
    for (int i = 0; i < password_len; i++)
    {
        int rn = (rand() % 93) + 33;
        if (rn <= 33 && rn >= 126){
            continue;
            i--;
        }
        password[i] = (char)rn;
    }
    password[password_len] = '\0';
    return 0;
}

