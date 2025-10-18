#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "types.h"



SearchCredsOutput search_record(char *query){
    FILE *fptr = fopen(VAULT_PATH, "rb");
    VaultEntry current_record;
    size_t query_len = strlen(query);
    int index = 0;
    int total_found =0;
    int *found_indices ;
    printf("\n%-5s %-20s %-20s\n", "ID", "Website", "Username");
    printf("---------------------------------------------------------\n");
    while(fread(&current_record,sizeof(current_record),1,fptr)==1){

        if ( strstr(current_record.website,query) != NULL || strstr(current_record.username, query) != NULL ) {
            printf("%-5d %-20s %-20s \n", index+1, current_record.website,current_record.username);
            found_indices = (int*)realloc(found_indices,total_found*sizeof(char));
            found_indices[total_found] = index;
            total_found++;
        }
        
        index++;
    }   
    printf("---------------------------------------------------------\n");
    printf("Total found: %d\n", total_found);

    SearchCredsOutput output = {total_found,found_indices,index};
    return output;
}


int read_vault(char *dek ){
    bool is_filters =false ;
    printf("1.List all stored credentials [0] \n2.Search by website/username [1]\n");
    scanf("%d",&is_filters);
    FILE *fptr ;
    int total =0;
    SearchCredsOutput search_output ;
    

    
    if (is_filters){
        char query[64];
        printf("Search by username or wesbite : ");
        getchar();
        fgets(query,sizeof(query),stdin);
        query[strcspn(query, "\n")] = '\0';
        search_output =  search_record(query);
        total = search_output.total_records;
    }
    else {
    printf("\n%-5s %-20s %-20s\n", "ID", "Website", "Username");
    printf("---------------------------------------------------------\n");
        fptr =  fopen(VAULT_PATH, "rb");
           if (fptr == NULL){  
            perror("File open failed");
            return -1;
        }

        VaultEntry current_record;
        while (fread(&current_record, sizeof(current_record), 1, fptr) == 1){
            printf("%-5d %-20s %-20s\n", total + 1, current_record.website, current_record.username);
            total++;
        }
        printf("---------------------------------------------------------\n");
        printf("Total entries: %d\n", total);
        fclose(fptr);
    }
        
        int target_index = 0;
        if (total > 0) {
            while (1){
                printf("Enter an ID to reveal the password (-1 to exit reading ) : ");
                scanf("%d",&target_index);
                
                if (target_index ==-1){
                    return 0;
                }
                
                // if(is_filters){
                //     bool is_found = false;
                //     for (int i = 0; i < total; i++){
                //         if(search_output.found_indices[i] == target_index){
                //             is_found =true;
                //             break;
                //         }
                //     }
                //     if(!is_found){
                //         printf("Select ID from the searched results \n");
                //         continue;
                //     }
                // }
                
                    if (target_index >total ||target_index <= 0){
                        printf("Invalid ID \n");
                    }
                    else break;
                
            }
            fptr = fopen(VAULT_PATH,"rb");

            fseek(fptr,(target_index-1)*sizeof(VaultEntry),SEEK_CUR);

            VaultEntry current_record;
            if(fread(&current_record, sizeof(current_record), 1, fptr) == 1){
                char plain_password[MAX_PASS_LEN] ;
                printf("\n%-5s %-20s %-20s %-20s\n", "ID", "Website", "Username","Password");
                printf("---------------------------------------------------------\n");
                int decryption_status = decrypt_vault_password(plain_password,&current_record.pwd,dek);                
                if(decryption_status!=0) return 1;
                printf("%-5d %-20s %-20s %-20s\n", target_index, current_record.website,current_record.username,plain_password);
                printf("---------------------------------------------------------\n");
            }
            else {
                perror("Error reading the record"); 
                return 1;
            }
        }
        else {
            printf("No entries found.\n");
        }
        return 0;
}




int write_record(char *dek){
    
    VaultEntry user_input ;
    while (1){
    printf("Type Username (type ' - ' for empty username input): ");
    getchar();
    fgets(user_input.username,sizeof(user_input.username),stdin);
    user_input.username[strcspn(user_input.username, "\n")] = '\0';
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
    if(strlen(user_input.website) > sizeof(user_input.website)){
        printf("Only 64 characters are allowed\n");
        continue;
    }
    
    break;
}
char plain_text_pass[MAX_PASS_LEN];
while (1){
    printf("Type Password (required) : ");
    
    fgets(plain_text_pass,MAX_PASS_LEN,stdin);
    plain_text_pass[strcspn(plain_text_pass, "\n")] = '\0';

    if (strlen(plain_text_pass) == 0){
        printf("password is required \n");
        continue;
    }
  
    int password_strength_status = check_password_strength(plain_text_pass);
    if(password_strength_status == 4){
        printf("At least 8 character password is required\n");
        continue;
    }
    else{
        bool update_pass = false;
        int is_change_password = 0;
        printf("\n-Change the password [1] \n-Keep it [0 or anything]\n ");
        scanf("%d",&is_change_password);
        if (is_change_password ==1) continue;
        else break;
    }
    
}
int encrypt_pass_status =  encrypt_vault_password(plain_text_pass,&user_input.pwd,dek);
if(encrypt_pass_status!=0){
    printf("Failed to store the credentials");
    return -1;
}


FILE *fptr = fopen(VAULT_PATH,"ab");

if (fptr ==NULL){
    perror("Error accessing to file . Try again \n");
}


// ! Encryption is required

if (fwrite(&user_input,sizeof(user_input),1,fptr)==1){
    printf("Successfully added!\n");
}

if (ferror(fptr)) {
    perror("Error adding credentials to file . Try again \n");
    return -1;
}

fclose(fptr);

return 0;
}



// int update_record(char *dek, char *file_path){
//     char query[256];
//     int target_index =0;
//     SearchCredsOutput search_result ;
//     while (1){
        
        
//         printf("Seach either by username or website url in the directory \nSearch :");
//         fgets(query,sizeof(query),stdin);
        
        
//         search_result = search_record("dek","/data/vault.dat",query);
        
//         if(search_result.total_found ==0){
//             printf("Not found any matching record\n");
//             continue;
//         }
//         int search_again = 0;
//             printf("\n-Search again [1] \n-Found [0 or anything]\n ");
//             scanf("%d",&search_again);
//             if (search_again ==1) continue;
//             else break;
//     }


//     bool is_valid_index = false;
//     while (!is_valid_index){
        
//     printf("Write the index of the record to update\n");
//     scanf("%d",&target_index);
//     for (int i = 0; i < search_result.total_found ; i++){
//         if(search_result.found_indices[i]==target_index) {
//             is_valid_index=true;
//             break;
//         }
//     }
//     if(!is_valid_index){
//         printf("Select valid index from above\n");
//     }
//     }

//     FILE *fptr = fopen(file_path,"rb+");
//     if(target_index !=0) fseek(fptr,target_index-1 ,SEEK_SET);
//     VaultEntry current_stored_record;
//     VaultEntry current_updated_record;
//     char plain_password[28];
//     bool change_username =true,change_password =true , change_wesbite =true;

//     fread(&current_stored_record,sizeof(current_stored_record),1,fptr);
//     while (1){
//         printf("Type the updated username (type 0 to remain same) : ");
//         fgets(&current_updated_record.username,sizeof(current_updated_record.username),stdin);
//         if(current_updated_record.username[0]=='0'){
//             change_username = false;
//             strcmp(current_updated_record.username,current_stored_record.username);
//             break;
//         }
//         else if(strcmp(current_updated_record.username," ")==0){
//             printf("Invalid username");
//             continue;
//         }
//         break;
//     }
//     while (1){
//         printf("Type the updated website url (type 0 to remain same) : ");
//         fgets(&current_updated_record.website,sizeof(current_updated_record.website),stdin);
//         if(current_updated_record.website[0]=='0'){
//             change_wesbite = false;
//             strcmp(current_updated_record.website,current_stored_record.website);
//             break;
//         }
//         else if(strcmp(current_updated_record.website,' ')==0){
//             printf("Invalid website url");
//             continue;
//         }
//         break;
//     }
//     while (1){
//         printf("Change Password (0/1)? : ");
//         printf("%d",&change_password);
//         if(change_password ==0){
//         change_password = false;
//         strcmp(current_updated_record.password,current_stored_record.password);
//         break;
//         }
//         take_password_input(&current_updated_record.password,sizeof(current_updated_record.password),"Type the updated password : ");
//         strcpy(plain_password,current_updated_record.password);
//         //Encrypt password 
//     }

//     printf("Are you sure you want to update ?");
//     printf("%-10s %10s %10s","Username","website url","password");
//     printf("%-10s %10s %10s",current_updated_record.username,current_updated_record.website,change_password ==0?"No change": plain_password);
//     if(!change_password && !change_username &&!change_wesbite) {
//         printf("Nothing to change");
//         return 0;
//     }
//     if (fwrite(&current_updated_record,sizeof(current_updated_record),1,fptr)!=1){
//         printf("Error updating , try again");
//         return 1;
//     };    
//     fclose(fptr);



// return 0;
// }



int operation_executer(unsigned char dek[crypto_secretbox_KEYBYTES]){
    
    while (1){
        int operation_num = 0;
        printf("=== Password Vault Menu ===\n");
        printf("1. Add a new password\n");
        printf("2. View saved passwords\n");
        printf("3. Update an existing password\n");
        printf("4. Delete a password\n");
        printf("\nSelect an option (1-4): ");

        scanf("%d",&operation_num);
        switch (operation_num){
            case 1:
            write_record(dek);
            break;
            case 2:
            read_vault(dek);
            break;
            default:
            printf("Invalid operation is selected\n");
            continue;
            break;
        }
    }
}