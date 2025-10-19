#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "types.h"

int update_record(unsigned char *dek ){
    
    int target_index =0;
    int is_search =0;
    printf("Do you want to search the record to select ID (0/1) : ");
    scanf("%d",&is_search);
    while (getchar()!='\n');

    if (is_search)  {
        SearchCredsOutput search_result = trigger_search();
    }
    


    
    FILE *fptr ;
    
    VaultEntry current_stored_record;
    while (1){
        bool is_update_record;
        printf("Write ID of the record to update (-1 to exit editing) : ");
        scanf("%d",&target_index);
        while (getchar()!='\n');
        if(target_index == -1){
            return 0;
        }
        if (target_index <= 0){
            printf("Invalid ID \n");
            continue;
        }
        

        fptr = fopen(VAULT_PATH,"rb+");

        fseek(fptr,(target_index-1) * sizeof(VaultEntry), SEEK_CUR);
        printf("\n%-5s %-20s %-20s\n", "ID", "Website", "Username");
        printf("---------------------------------------------------------\n");
        if(fread(&current_stored_record,sizeof(current_stored_record),1,fptr)==1 && !current_stored_record.is_deleted){
            print_record(current_stored_record,target_index,false,"");
            printf("---------------------------------------------------------\n");
        }
        else {
            perror("Unable to read the selected record (Invalid ID) \n");
            continue;
        }
        printf("Are you sure to update this record (0/1) : ");
        scanf("%d",&is_update_record);
        while (getchar()!='\n');
        
        if(is_update_record){
            break;
        }
        else {
            return 0;
        }
    }


    if(target_index !=0) fseek(fptr,target_index-1 ,SEEK_SET);
    
    VaultEntry current_updated_record;

    
    bool change_username =true,change_password =true , change_wesbite =true;

    
    while (1){

        printf("Type the updated username (type 0 to remain unchanged) : ");

        fgets(current_updated_record.username,sizeof(current_updated_record.username),stdin);
        current_updated_record.username[strcspn(current_updated_record.username, "\n")] = '\0';

        if(current_updated_record.username[0]=='0'||current_updated_record.username[0]==' '){
            change_username = false;
            strcpy(current_updated_record.username,current_stored_record.username);
            break;
        }
        else if(strlen(current_updated_record.username)<3){
            printf("At least 3 characters are required for username \n");
            continue;
        }
        break;
    }
    while (1){
        printf("Type the updated website url (type 0 to remain unchanged) : ");
        fgets(current_updated_record.website,sizeof(current_updated_record.website),stdin);

        current_updated_record.website[strcspn(current_updated_record.website, "\n")] = '\0';
        if(current_updated_record.website[0]=='0' ||current_updated_record.website[0]==' '){
            change_wesbite = false;
            strcpy(current_updated_record.website,current_stored_record.website);
            break;
        }
        else if(strlen(current_updated_record.website)<3){
            printf("At least 3 characters are required for website url \n");
            continue;
        }
        break;
}
    char plain_password_input[MAX_PASS_LEN];
    while (1){
        int change_password=0;
        printf("Update Password (0/1)  : ");
        scanf("%d",&change_password);
        while (getchar() != '\n');
        if(change_password ==0){
        change_password = false;
        memcpy(&current_updated_record.pwd,&current_stored_record.pwd,sizeof(Encrypted_Password));
        break;
        }
    take_password_input(plain_password_input,sizeof(plain_password_input),"Type the updated password : ",false);
    //Encrypt password 
    int encrypt_pass_status =  encrypt_vault_password(plain_password_input,&current_updated_record.pwd,dek);
    if(encrypt_pass_status!=0){
        perror("Failed to store the credentials\n");
        return -1;
    }
    break;
    }

    if(!change_password && !change_username &&!change_wesbite) {
        printf("Nothing to update\n");
        return 0;
    }

    if (fwrite(&current_updated_record,sizeof(current_updated_record),1,fptr)!=1){
        printf("Failed to write updated record to file\n");
        return 1;
    };    
    fclose(fptr);
    printf("Record updated successfully");

    return 0;
} 

