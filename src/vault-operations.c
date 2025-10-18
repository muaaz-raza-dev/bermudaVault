#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "types.h"


void display_record(VaultEntry record,int index,bool is_password,char *plain_password){
    printf("%-5d %-20s %-20s ", index, record.website,record.username,is_password);
    if(is_password) printf("%-20s",plain_password);
    printf("\n");
}

SearchCredsOutput search_record(char *query){
    FILE *fptr = fopen(VAULT_PATH, "rb");
    if(fptr == NULL){
        perror("Error opening the file");
    }

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
    if(total_found ) printf("---------------------------------------------------------\n");
    printf("Total found: %d\n", total_found);

    SearchCredsOutput output = {total_found,found_indices,index};
    fclose(fptr);
    return output;
}

SearchCredsOutput trigger_search(){
    SearchCredsOutput search_result;
    char query[64];
    while (1){
        printf("Search record by username or website : ");
        getchar() ;
        fgets(query,sizeof(query),stdin);
        query[strcspn(query, "\n")] = '\0';

        search_result = search_record(query);
        
        if(search_result.total_found ==0){
            printf("Not found any matching record\n");
            continue;
        }
        break;
    }
    return search_result;
}

int read_vault(unsigned char *dek ){
    int read_type = 1 ;
    printf("1.Read all \n2.Search by website/username \n3.Access directly by ID\n4.Exit Read\n");
    scanf("%d",&read_type);
    FILE *fptr ;
    int total =0;
    SearchCredsOutput search_output ;
    

    
    if (read_type==2){
        while (!total){
            char query[64];
            printf("Search by username or wesbite (Enter 0 to exit reading): ");
            getchar();
            fgets(query,sizeof(query),stdin);
            if(query[0]=='0') return 0;
            query[strcspn(query, "\n")] = '\0';
            search_output =  search_record(query);
            total = search_output.total_records;
        }
    }
    else if(read_type==1) {
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
    else if(read_type==3){
        int target_index = 0;
        printf("Enter ID of the record : ");
        scanf("%d",&target_index);

        printf("\n%-5s %-20s %-20s\n", "ID", "Website", "Username");
        printf("---------------------------------------------------------\n");
        fptr =  fopen(VAULT_PATH, "rb");
           if (fptr == NULL){  
            perror("File open failed");
            return -1;
        }

        VaultEntry current_record;
        fseek(fptr,(target_index-1 )* sizeof(VaultEntry),SEEK_CUR);
        if(fread(&current_record, sizeof(current_record), 1, fptr) == 1){
            printf("%-5d %-20s %-20s\n", total + 1, current_record.website, current_record.username);
            printf("---------------------------------------------------------\n");
        }
        else {
            printf("Error reading record (Invalid ID)");
            return -1;
        }
        fclose(fptr);
    }
        
        int target_index = 0;
        if (total > 0) {
            while (1){
                printf("Enter record ID to reveal the password (-1 to exit reading ) : ");
                scanf("%d",&target_index);
                if(target_index==-1){
                    return 0;
                }
                if ((read_type!=3 && target_index >total )||target_index <= 0){
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
                perror("Error reading record (Invalid ID or file crashed)\n"); 
            }
        }
        else {
          if(read_type!=3)  printf("No entries found.\n");
        }
        return 0;
}




int write_record(unsigned char *dek){
    
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






int operation_executer(unsigned char dek[crypto_secretbox_KEYBYTES]) {
    while (1) {
        int operation_num = 0;

        printf("\n=== Password Vault Menu ===\n");
        printf("1. Add a new record\n");
        printf("2. Read saved records\n");
        printf("3. Update an existing record\n");
        printf("4. Delete a password\n");
        printf("5. Change master password\n");
        printf("6. Exit\n");
        printf("\nSelect an option (1-6): ");

        if (scanf("%d", &operation_num) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // clear the input buffer
            while (getchar() != '\n');
            continue;
        }

        switch (operation_num) {
            case 1:
                write_record(dek);
                break;

            case 2:
                read_vault(dek);
                break;

            case 3:
                update_record(dek);
                break;

            case 4:
                // delete_record(dek);
                break;

            case 5:
                change_master_password(dek);
                break;

            case 6:
                printf("Exiting vault... Goodbye!\n");
                return 0;

            default:
                printf("Invalid operation selected. Please try again.\n");
                continue;
        }
    }
}
