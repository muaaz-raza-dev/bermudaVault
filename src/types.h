#ifndef TYPES_H
#define TYPES_H
#define SALT_LEN 16
#define MAX_PASS_LEN 64
#define MAX_PASSWORD_CIPHER_LEN (MAX_PASS_LEN + crypto_secretbox_MACBYTES)
#define VAULT_PATH "./data/vault.dat"
#define DEK_LEN crypto_aead_xchacha20poly1305_ietf_KEYBYTES
#pragma once
#include <sodium.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    unsigned char salt[SALT_LEN];              
    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES]; 
    uint32_t ciphertext_len;                                  
    unsigned int  ciphertext[crypto_secretbox_MACBYTES + crypto_secretbox_KEYBYTES];                               
} VaultHeader;

typedef struct {
    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES]; 
    unsigned long long ciphertext_len;                                  
    unsigned char ciphertext[MAX_PASSWORD_CIPHER_LEN];
} Encrypted_Password;

typedef struct {
    Encrypted_Password pwd;
    char username[64];
    char website[64];
} VaultEntry;

typedef struct {
    int total_found;
    int total_records;
    int *found_indices;
} SearchCredsOutput;

int check_password_strength(char *pass);
void print_hex(const char *label, const unsigned char *data, size_t len) ;
int authenticate(unsigned char DEK[crypto_secretbox_KEYBYTES]);
int generate_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES],bool is_update_password);

int take_password_input(char *pass,size_t pasword_limit,char *print_message,bool is_strict);

int load_dek(char *PASSWORD,unsigned char DEK[crypto_secretbox_KEYBYTES]);
int decrypt_vault_password(char plain_text_password[MAX_PASS_LEN],Encrypted_Password *pwd,unsigned char dek[crypto_secretbox_KEYBYTES]);
int change_master_password(unsigned char *DEK);

int operation_executer(unsigned char dek[crypto_secretbox_KEYBYTES]);

int encrypt_vault_password(char plain_text_password[MAX_PASS_LEN],Encrypted_Password *pwd,unsigned char dek[crypto_secretbox_KEYBYTES]);

void display_record(VaultEntry record,int index,bool is_password,char *plain_password);
SearchCredsOutput search_record(char *query);
SearchCredsOutput trigger_search();

int update_record(unsigned char *dek );






#endif // TYPES_H