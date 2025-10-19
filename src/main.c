#include <stdio.h>

#include "types.h"
#include <unistd.h>


#include <sodium.h>





int main(){
    unsigned char dek[crypto_secretbox_KEYBYTES];
    printf("\033[1;36m=============== Bermuda Vault 1.0 ===============\033[0m\n");


    if (sodium_init() < 0) {
    printf("libsodium init failed.\n");
    return -1;
    }
    authenticate(dek);
    operation_executer(dek);
    


}