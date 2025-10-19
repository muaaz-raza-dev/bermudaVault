#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sodium.h>
#include "types.h"
#include <time.h>




int check_password_strength(char *pass){ 
    // 0 success , 4 too short 3 
    int i = 0;
    
    bool is_small_case = false,is_upper_case = false, is_symbols = false , is_numbers = false;
    while (pass[i] != '\0' && pass[i] != '\n'){
        int ascii_value = (int)pass[i];
        if ((ascii_value >= 32 && ascii_value <= 47) || ascii_value == 95 || ascii_value == 96 || ascii_value == 0|| (ascii_value >= 123 && ascii_value <= 126)){
            is_symbols= true;
        }
        else if(ascii_value >= 48 && ascii_value <= 57){ is_numbers =true;}
        else if (ascii_value >= 65 && ascii_value <= 90){ is_small_case =true;}
        else if (ascii_value >= 97 && ascii_value <= 122){is_upper_case=true;}
        i++;
    }
    int length = i + 1;
    
    if(length < 8) {
        printf("Password is too short, atleast 8 characters are required. \n");
        return 4;
    }
    
    int strength_level = is_numbers + is_small_case + is_upper_case + is_symbols;

    if (strength_level >= 4){
        printf("Password is strong");
        return 0;
    }
    else if(strength_level ==3){
        printf("Password is just good ! ");
        return 1;
    }
    else if(strength_level ==2){
        printf("Password is weak ! ");
        return 2;
    }
    else {
        printf("Password is too weak ! ");
        return 3;
    }
}

int take_password_input(char *pass,size_t pasword_limit,char *print_message,bool is_strict){
    bool is_first_time = true;
    while (1){
           printf("%s",print_message);
            if (!is_first_time){
                getchar();
            } 
            is_first_time = false;
            fgets(pass,pasword_limit,stdin);
            pass[strcspn(pass, "\n")] = '\0';

            int strength_status = check_password_strength(pass); 
            if (is_strict){
                if (strength_status == 4){continue;}
                if(strength_status == 0){ 
                    break;
                }
            }
            else{
                int is_change_password = 0;
                printf("\n Change the password (0/1) : ");
                scanf("%d",&is_change_password);
                while(getchar()!='\n');
                if (is_change_password == 1) continue;
                else break;
            }
    }
    return 0;
}

int generate_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES],bool is_update_password){
    mkdir("./data", 0700);
    FILE *fptr = fopen("./data/user.dat","rb+");
    

    if (fptr == NULL){
        printf("Not found");
        return 0;
    }
    
    unsigned char salt[SALT_LEN] ;
    unsigned char kek[crypto_secretbox_KEYBYTES];

    randombytes_buf(salt, SALT_LEN);
    if(crypto_pwhash (kek, sizeof(kek), PASSWORD, strlen(PASSWORD), salt,
    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
    crypto_pwhash_ALG_DEFAULT)!=0){
        printf("Error occured at hashing password ");
        exit(1);
    }

    
    unsigned char dek[crypto_secretbox_KEYBYTES];
    if(!is_update_password){
        randombytes_buf(dek, sizeof(dek));
    }
    else memcpy(dek,DEK,crypto_secretbox_KEYBYTES);

    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    unsigned char encrypted_dek[crypto_secretbox_MACBYTES + sizeof dek];
    if( crypto_secretbox_easy(encrypted_dek, dek, sizeof(dek), nonce, kek) !=0){
        printf("Failed to encrypt DEK");
        exit(1);
    }


    
    if(!is_update_password) memcpy(DEK, dek,crypto_secretbox_KEYBYTES );


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
                      exit(1);
        }

    
    
    
    
    if (crypto_secretbox_open_easy(DEK, vault.ciphertext, vault.ciphertext_len, vault.nonce, kek) != 0) {
        printf("Incorrect password \n");
        return -1;
    }
    return 0;
}

void generate_secure_password(char *password, size_t length) {
    const char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#$%^&*()_+-={}[]<>?";

    unsigned char random_bytes[length];
    randombytes_buf(random_bytes, length);

    for (size_t i = 0; i < length - 1; i++) {
        password[i] = charset[random_bytes[i] % (sizeof(charset) - 1)];
    }
    password[length - 1] = '\0';
}

