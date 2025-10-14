#include <stdio.h>
#include "types.h"
#include <unistd.h>

int main(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Current working directory: %s\n", cwd);
    unsigned char dek[crypto_secretbox_KEYBYTES];
    printf("Bermuda Vault 1.0\n");
    authenticate(dek);
}