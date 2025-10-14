#ifndef TYPES_H
#define TYPES_H
#define SALT_LEN 16
#define DEK_LEN crypto_aead_xchacha20poly1305_ietf_KEYBYTES
#pragma once
#include <sodium.h>
#include <stdint.h>
typedef struct {
    unsigned char salt[SALT_LEN];              
    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES]; 
    uint32_t ciphertext_len;                                  
    unsigned char ciphertext[];                               
} VaultHeader;


int check_password_strength(char *pass);
int authenticate(unsigned char DEK[crypto_secretbox_KEYBYTES]);
int generate_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES]);
int load_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES]);




void generate_random_salt(unsigned char *salt) ;
#endif // TYPES_H