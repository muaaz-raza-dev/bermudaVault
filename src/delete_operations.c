#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"
#include <errno.h>




int delete_record(unsigned char *dek){
    while (1){
    
    int del_read_type = 1;
    int total = 0;
    int target_index = 0;
    bool found_target_index = false;
    FILE *fptr = fopen(VAULT_PATH,"rb+");

    while (!found_target_index ){
        printf("\n\033[1;36m================= Vault Delete Menu =================\033[0m\n");
        printf("  \033[1;33m[1]\033[0m  Search and select ID\n");
        printf("  \033[1;33m[2]\033[0m  Enter record ID\n");
        printf("  \033[1;33m[3]\033[0m  Exit\n");
        printf("\033[1;36m------------------------------------------------------\033[0m\n");
        printf("\033[1;32mSelect an option (1-3): \033[0m");

        
        scanf("%d",&del_read_type);
        if(del_read_type < 0 || del_read_type > 2){
            return 0;
        }

        if(del_read_type ==1){
            char query[64];
            printf("Search by username or wesbite (Enter 0 to exit searching) : ");
            getchar();
            fgets(query,sizeof(query),stdin);
            query[strcspn(query, "\n")] = '\0';
            if(strlen(query) ==1 && query[0]=='0') return 0;
            SearchCredsOutput search_output =  search_record(query);
            total = search_output.total_records;
        }

        while (1){
            printf("Enter ID of the record (-1 to exit searching) : ");
            scanf("%d",&target_index);
            if(target_index == -1){
                break;
            }
            if(target_index <= 0){
                printf("Invalid ID\n");
                continue;
            }
            int display_decrypted_record_status = display_decrypted_record(dek,total,fptr,target_index);
            if(display_decrypted_record_status != 0) continue;
            
            int del_current_record = 0;
            printf("Are you sure you want to delete this record ? (1/0) : ");
            scanf("%d",&del_current_record);
            if(!del_current_record) break;
            found_target_index = true;
            break;
        }
    }
    
    fseek(fptr, (target_index-1)*sizeof(VaultEntry), SEEK_SET);
    VaultEntry current_record ;
    
    
    
    int deleted_records_count = 0;
    int *deleted_record_indices = NULL;
    if(fread(&current_record,sizeof(VaultEntry),1,fptr)!=1){
        fclose(fptr);
        printf("Error reading to the selected record\n");
        return 1;
    }
    
    current_record.is_deleted = true;
    fseek(fptr, (target_index -1 )*sizeof(VaultEntry), SEEK_SET);
    if (fwrite(&current_record,sizeof(VaultEntry),1,fptr) !=1){
            printf("Error writing to the given ID record\n");
            fclose(fptr);
            return 1;    
    }
    


        FILE *del_log_fptr = fopen(VAULT_DELETE_LOG_PATH,"rb+");
        if (!del_log_fptr) {
            recheck_empty_deleted_spaces();
            del_log_fptr = fopen(VAULT_DELETE_LOG_PATH, "rb+");
            if(!del_log_fptr){
                perror("Failed to read vault log file");
                return 1;
            }
    }   

        if (fread(&deleted_records_count,sizeof(int),1,del_log_fptr) == 1){
                deleted_record_indices = malloc((deleted_records_count + 1) * sizeof(int));
                fread(deleted_record_indices, sizeof(int), deleted_records_count, del_log_fptr);
                deleted_record_indices[deleted_records_count] = target_index -1;
                deleted_records_count++;
            }
            else {
            deleted_records_count =1;
            deleted_record_indices = malloc(sizeof(int));
            deleted_record_indices[0] = target_index -1;
            }
            
    fseek(del_log_fptr, 0, SEEK_SET);
    fwrite(&deleted_records_count, sizeof(int), 1, del_log_fptr);
    fwrite(deleted_record_indices, sizeof(int), deleted_records_count, del_log_fptr);
    
    fclose(fptr);    
    fclose(del_log_fptr);    

    printf("Record is deleted successfully !\n");
    int is_continue = ask_yes_no("Delete another record");
    if(is_continue != 1 ){break;}
    }
return 0;
}





int recheck_empty_deleted_spaces(void) {
    FILE *fptr = fopen(VAULT_PATH, "rb");
    if (!fptr) {
        perror("Failed to open vault file");
        return 1;
    }

    VaultEntry current_record;
    int *deleted_indices = NULL;
    int total_deleted_spaces = 0;
    int record_index = 0;

    while (fread(&current_record, sizeof(current_record), 1, fptr) == 1) {
        if (current_record.is_deleted) {
            total_deleted_spaces++;
            int *temp = realloc(deleted_indices, total_deleted_spaces * sizeof(int));
            if (!temp) {
                perror("Memory allocation failed");
                free(deleted_indices);
                fclose(fptr);
                return 1;
            }
            deleted_indices = temp;
            deleted_indices[total_deleted_spaces - 1] = record_index;
        }
        record_index++;
    }
    fclose(fptr);

    FILE *fptr_del_log = fopen(VAULT_DELETE_LOG_PATH, "wb");
    if (!fptr_del_log) {
        perror("Failed to open delete log file");
        free(deleted_indices);
        return 1;
    }

    fwrite(&total_deleted_spaces, sizeof(int), 1, fptr_del_log);
    fwrite(deleted_indices, sizeof(int), total_deleted_spaces, fptr_del_log);

    fclose(fptr_del_log);
    free(deleted_indices);

    return 0;

}
