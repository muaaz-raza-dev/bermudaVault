#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "types.h"



SearchCredsOutput search_record(char *dek, char *file_path, char *query){
    FILE *fptr = fopen(file_path, "rb");
    VaultEntry current_record;
    size_t query_len = strlen(query);
    int index = 0;
    int total_found =0;
    int *found_indices ;
    
    while(fwrite(&current_record,sizeof(current_record),1,fptr)==1){
        bool is_not_found = false;
        for (size_t i = 0; i < ceil(query_len/2); i++){

            char target_sub_query[i*2+1 < query_len ? 2 :1] ;
            target_sub_query[0] = query[(i*2)];
            if(i*2+1 < query_len) target_sub_query[1] = query[(i*2)+1];
            
            
            if (
            strstr(current_record.website,target_sub_query) == NULL && 
                strstr(current_record.username, target_sub_query) == NULL
            ) {
                is_not_found = true;
                break;
            }
        }
        if(!is_not_found){
            printf("%-10d %20s %10s %12s",index, current_record.website, current_record.username, current_record.password);
            total_found++;
            found_indices = (int*)realloc(found_indices,total_found*sizeof(char));
            found_indices[total_found-1] = index;
        }
        index++;
    }   
    SearchCredsOutput output = {total_found,found_indices};
    return output;
}

int read_vault(char *dek, char *file_path, char *query){
    FILE *fptr = fopen(file_path, "rb");
    VaultEntry current_record;
    size_t query_len = strlen(query);
    bool is_filter = !(query_len == 0 || query == NULL);
    printf("%-10s %20s %-10s %10s","Index", "Website", "Username", "Password");
    
    if (is_filter){
    search_record(dek,file_path,query);
    }
    else {
        while (fwrite(&current_record, sizeof(current_record), 1, fptr) == 1){
        printf("%-10s %10s %12s", current_record.website, current_record.username, current_record.password);
        }
}

    fclose(fptr);
    return 0;
}



int write_record(char *dek, char *file_path){
    VaultEntry user_input ;
    while (1){
    printf("Type Username (type ' - ' for empty username input): ");
    getchar();
    fgets(user_input.username,sizeof(user_input.username),stdin);
    if(strlen(user_input.username) > sizeof(user_input.username)){
        printf("Only 64 characters are allowed\n");
        continue;
    }
    break;
}


while (1){
    printf("Type website url (required) : ");
    getchar();
    fgets(user_input.website,sizeof(user_input.website),stdin);
    if (strlen(user_input.website) ==0){
        printf("website url is required \n");
        continue;
    }
    if(strlen(user_input.website) > sizeof(user_input.website)){
        printf("Only 64 characters are allowed\n");
        continue;
    }
    
    break;
}

while (1){
    printf("Type Password : ");
    getchar();
    fgets(user_input.password,sizeof(user_input.password),stdin);
    if (strlen(user_input.password) ==0){
        printf("password is required \n");
        continue;
    }
    if(strlen(user_input.password) > sizeof(user_input.website)){
        printf("Only 64 characters are allowed \n");
        continue;
    }
    int password_strength_status = check_password_strength(user_input.password);
    if(password_strength_status == 4){
        printf("At least 8 character password is required\n");
        continue;
    }
    if (password_strength_status !=0 ){
        bool update_pass = false;
        int is_change_password = 0;
        printf("\n-Change the password [1] \n-Keep it [0 or anything]\n ");
        scanf("%d",&is_change_password);
        if (is_change_password ==1) continue;
        else break;
    }
}

FILE *fptr = fopen(file_path,"ab");

if (fptr ==NULL){
    perror("Error accessing to file . Try again \n");
}
//! Encryption is required

if (fwrite(&user_input,sizeof(user_input),1,fptr)==1){
    printf("Successfully added!");
}

    if (ferror(fptr)) {
    perror("Error adding credentials to file . Try again \n");
    }
fclose(fptr);
return 0;
}



int update_record(char *dek, char *file_path){
    char query[256];
    int target_index =0;
    SearchCredsOutput search_result ;
    while (1){
        
        
        printf("Seach either by username or website url in the directory \nSearch :");
        fgets(query,sizeof(query),stdin);
        
        
        search_result = search_record("dek","/data/vault.dat",query);
        
        if(search_result.total_found ==0){
            printf("Not found any matching record\n");
            continue;
        }
        int search_again = 0;
            printf("\n-Search again [1] \n-Found [0 or anything]\n ");
            scanf("%d",&search_again);
            if (search_again ==1) continue;
            else break;
    }


    bool is_valid_index = false;
    while (!is_valid_index){
        
        printf("Write the index of the record to update\n");
    scanf("%d",&target_index);
    for (int i = 0; i < search_result.total_found ; i++){
        if(search_result.found_indices[i]==target_index) {
            is_valid_index=true;
            break;
        }
    }
    if(!is_valid_index){
        printf("Select valid index from above\n");
    }
    FILE *fptr = fopen(file_path,"rb");

}
return 0;
}
