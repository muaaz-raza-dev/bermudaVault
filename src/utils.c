#include <stdio.h>
#include <stdbool.h>
#include "types.h"
void print_record(VaultEntry record,int index,bool is_password,char *plain_password){
    printf("%-5d %-20s %-20s ", index, record.website,record.username,is_password);
    if(is_password) printf("%-20s",plain_password);
    printf("\n");
}
void print_hex(const char *label, const unsigned char *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void print_vault_entries_header(bool is_password){
    if(is_password){
        printf("\n\033[1;36m%-5s %-20s %-20s %-20s\033[0m\n", "ID", "Website", "Username", "Password");
        printf("\033[1;36m--------------------------------------------------------------------------\033[0m\n");
    }
    else{
        printf("\n\033[1;36m%-5s %-20s %-20s\033[0m\n", "ID", "Website", "Username");
        printf("\033[1;36m------------------------------------------------------\033[0m\n");
    }
}
