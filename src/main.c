#include <stdio.h>
#include "types.h"
#include <unistd.h>
#include <sodium.h>

int main(){
    unsigned char dek[crypto_secretbox_KEYBYTES];
    printf("Bermuda Vault 1.0\n");
    if (sodium_init() < 0) {
    printf("libsodium init failed.\n");
    return -1;
    }
    authenticate(dek);
    printf("Bermuda Vault is ready to serve you \n ");
    printf("1.Read passwords\n");
    printf("2.Update passwords\n");
    printf("3.Delete password\n");
    printf("4.Add password\n");
}