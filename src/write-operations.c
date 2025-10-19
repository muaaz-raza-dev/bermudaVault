#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"
#include <errno.h>


int write_record(unsigned char *dek){
    printf("\n---------------------------------------------\n");
    printf(" Tip: Enter 0 at any input to cancel or exit writing.\n");
    printf("---------------------------------------------\n");

    VaultEntry user_input ;
    user_input.is_deleted =false;
    while (1){
    printf("Type Username (type ' - ' for empty username input) : ");
    while (getchar() != '\n');
    fgets(user_input.username,sizeof(user_input.username),stdin);
    user_input.username[strcspn(user_input.username, "\n")] = '\0';
    if(strlen(user_input.username) == 1 && user_input.username[0]=='0'){
        return 0;
    }
    if (strcmp(user_input.username,"-") ==0){
    user_input.username[0] = '\0';
    }

    break;
}


while (1){
    printf("Type website url (required) : ");
    fgets(user_input.website,sizeof(user_input.website),stdin);
    user_input.website[strcspn(user_input.website, "\n")] = '\0';
    if (strlen(user_input.website) ==0){
        printf("website url is required \n");
        continue;
    }
    else if(strlen(user_input.website) == 1 && user_input.website[0]=='0'){
        return 0;
    }
    break;
}
char plain_text_pass[MAX_PASS_LEN];
while (1){
    printf("Enter password (required)\n");
    printf("[Enter 1 to auto-generate a strong password]: ");
    bool auto_gen_pass = false;
    fgets(plain_text_pass,MAX_PASS_LEN,stdin);
    plain_text_pass[strcspn(plain_text_pass, "\n")] = '\0';

    if (strlen(plain_text_pass) == 0){
        printf("password is required \n");
        continue;
    }
    if(strlen(plain_text_pass) == 1 ){
        if( plain_text_pass[0]=='0'){
            return 0;
        }
        else if(plain_text_pass[0]=='1'){
            auto_gen_pass =true;
            generate_secure_password(plain_text_pass,16);
            printf("Auto Generated Password : %s\n",plain_text_pass);
        }
    }

        if(!auto_gen_pass) check_password_strength(plain_text_pass);
        int is_change_password = 0;
        printf("Change the password (1/0) : ");
        scanf("%d",&is_change_password);
        while (getchar() != '\n');
        if (is_change_password ==1) continue;
        else break;
    
}
int encrypt_pass_status =  encrypt_vault_password(plain_text_pass,&user_input.pwd,dek);
if(encrypt_pass_status!=0){
    printf("Failed to store the credentials");
    return -1;
}


FILE *fptr = fopen(VAULT_PATH, "rb+");
if (!fptr) {
    perror("Error opening the vault file");
    return 1;
}

FILE *fptr_del_log = fopen(VAULT_DELETE_LOG_PATH, "rb+");
if (!fptr_del_log) {
    if (errno == ENOENT) {
        recheck_empty_deleted_spaces();
    } else {
        perror("Error opening delete log");
        fclose(fptr);
        return 1;
    }
    fptr_del_log = fopen(VAULT_DELETE_LOG_PATH, "rb+");
    if (!fptr_del_log) {
        perror("Failed to recreate delete log");
        fclose(fptr);
        return 1;
    }
}

int total_empty_spaces = 0;
int last_empty_space_index = 0;
bool has_empty_space = false;

// Read count
fread(&total_empty_spaces, sizeof(int), 1, fptr_del_log);

if (total_empty_spaces > 0) {
    // Move to last index
    fseek(fptr_del_log, sizeof(int) * (total_empty_spaces - 1), SEEK_CUR);
    fread(&last_empty_space_index, sizeof(int), 1, fptr_del_log);
    has_empty_space = true;
}

if (has_empty_space) {
    // Overwrite deleted record slot
    fseek(fptr, last_empty_space_index * sizeof(VaultEntry), SEEK_SET);

    // Update delete log: decrease count
    total_empty_spaces--;
    fseek(fptr_del_log, 0, SEEK_SET);
    fwrite(&total_empty_spaces, sizeof(int), 1, fptr_del_log);

    // Shrink log file by 4 bytes (remove last index)
    fseek(fptr_del_log, 0, SEEK_END);
    long size_file = ftell(fptr_del_log);
    long new_size = (size_file > 4) ? (size_file - 4) : 0;

    if (TRUNCATE(FILENO(fptr_del_log), new_size) != 0) {
        perror("File truncation failed");
        fclose(fptr);
        fclose(fptr_del_log);
        return 1;
    }
}
else {
    fseek(fptr,0, SEEK_END);
}

if (fwrite(&user_input, sizeof(user_input), 1, fptr) == 1) {
    printf("Successfully added!\n");
} else {
    perror("Error adding credentials to file. Try again.");
}

fclose(fptr);
fclose(fptr_del_log);

return 0;
}