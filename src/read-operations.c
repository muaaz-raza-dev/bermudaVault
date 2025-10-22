#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"

int display_decrypted_record(unsigned char *dek,int total,FILE *fptr,int root_target_index){
    bool is_input_target_index = root_target_index==-1;
    int target_index = is_input_target_index ? 0 : root_target_index;
    
    if (is_input_target_index){
        while (1){
            printf("Enter record ID to reveal the password (-1 to exit reading ) : ");
            scanf("%d",&target_index);
            if(target_index==-1){
                return 0;
            }
            else if (target_index <= 0){
                printf("Invalid ID \n");
                continue;
            }
            
                break;
            }
        }
        fseek(fptr,(target_index-1)*sizeof(VaultEntry),SEEK_SET);

            VaultEntry current_record;
            if(fread(&current_record, sizeof(current_record), 1, fptr) == 1){
                if(current_record.is_deleted) {
                printf("Invalid ID \n"); 
                return -1;    
                }
                char plain_password[MAX_PASS_LEN] ;
                print_vault_entries_header(true);
                int decryption_status = decrypt_vault_password(plain_password,&current_record.pwd,dek);                
                if(decryption_status!=0) return 1;
                printf("%-5d %-20s %-20s %-20s\n", target_index, current_record.website,current_record.username,plain_password);
                printf("--------------------------------------------------------------------------\n");
            }
            else {
                perror("Error reading record (Invalid ID or file crashed)\n"); 
                return -1;
            }
            return 0;
}

SearchCredsOutput search_record(char *query){
    FILE *fptr = fopen(VAULT_PATH, "rb");
    if(fptr == NULL){
        perror("Error opening the file");
    }

    VaultEntry current_record;
    int index = 0;
    int total_found =0;
    int *found_indices = NULL;
    
    print_vault_entries_header(false);
    while(fread(&current_record,sizeof(current_record),1,fptr)==1 ){
        index++;
        if(current_record.is_deleted) continue;
        if ( strstr(current_record.website,query) != NULL || strstr(current_record.username, query) != NULL ) {
            printf("%-5d %-20s %-20s \n", index, current_record.website,current_record.username);
            found_indices = (int*)realloc(found_indices,(total_found+1)*sizeof(int));
            found_indices[total_found] = index-1;
            total_found++;
        }
        
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
    system("cls");
    while (read_type<4&&read_type>0){
    printf("\n\033[1;36m================= Vault Reader Menu =================\033[0m\n");
    printf("  \033[1;33m[1]\033[0m  View all records\n");
    printf("  \033[1;33m[2]\033[0m  Search by website or username\n");
    printf("  \033[1;33m[3]\033[0m  Access by ID\n");
    printf("  \033[1;33m[4]\033[0m  Exit to main menu\n");
    printf("\033[1;36m------------------------------------------------------\033[0m\n");
    printf("\033[1;32mSelect an option (1-4): \033[0m");



    scanf("%d",&read_type);
    FILE *fptr =fopen(VAULT_PATH, "rb");
    if (fptr == NULL){  
        printf("No records found or error occured while opening file");
        return 0;        
    } ;
    int total =0;
    SearchCredsOutput search_output ;

    
    if (read_type==2){
        while (!total){
            char query[64];
            printf("Search by username or wesbite (Enter 0 to exit reading): ");
            getchar();
            fgets(query,sizeof(query),stdin);
            query[strcspn(query, "\n")] = '\0';
            if(strlen(query) ==1 && query[0]=='0') return 0;
            search_output =  search_record(query);
            total = search_output.total_records;
        }
    }
    else if(read_type==1) {
        print_vault_entries_header(false);
        VaultEntry current_record;
        int index = 0;
        while (fread(&current_record, sizeof(current_record), 1, fptr) == 1){
            index++;
            if(current_record.is_deleted) continue;
            total++;
            printf("%-5d %-20s %-20s\n", index, current_record.website, current_record.username);
        }
        printf("---------------------------------------------------------\n");
        printf("Total entries: %d\n", total);
    }
    else if(read_type==3){
        int target_index = 0;
        printf("Enter ID of the record (-1 to exit reading): ");
        scanf("%d",&target_index);
        if(target_index == -1){
            continue;
        }
        display_decrypted_record(dek,total,fptr,target_index);
    }
        
        if(read_type!=3&& total >0 ) display_decrypted_record(dek,total,fptr,-1);
    }
        
        return 0;
}


