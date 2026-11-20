#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "helpers.h"



    int main(int argc, char *argv[]){
        bool watch_flag = false;
        FILE *stale_fileptr;
        // variable to track the file name index
        int file_pos = 0;
        // Validate cline arguments and print hit if invalid
        if (argc < 3 || argc > 4){
            printf("Invalid arguments provided %d.\nExpected:\n\t-f <filename> (requried)\n\t-w (watch flag, optional)\n", argc);
            return 4;
        }
        
        //Parse the arguments one by one
        for (int i= 0; i<argc; ++i){

            if (argv[i][0]=='-'){
                
                switch (argv[i][1])
                {
                case 'w':
                    watch_flag = true;
                    printf("Watch is not yet implemented!\n");
                    break;
                case 'f':
                    file_pos = i+1;
                    break;
                default:
                    printf("Invalid arguments provided '%c'.\nExpected:\n\t-f <filename> (requried)\n\t-w (watch flag, optional)\n", argv[i][1]);
                    break;
                }
            }
            
        }

        if(file_pos == 0){
                printf("No filename passed with '-f' flag.\n");
                return 5;
            }

            char *stale_file = sanitize(argv[file_pos]);
            printf("Sanitized file: %s \n", stale_file);
            // Generate the full path to stalefile
            char *base_dir = get_base_dir();
            if (base_dir == NULL){
                printf("Cannot allocate base_dir.\n");
                return -1;
            }
            size_t base_dir_len = strlen(base_dir);
            size_t len_sf = strlen(stale_file);
            //allocate enough memory for basedir+stale_file
            char *stale_file_f = malloc(base_dir_len+len_sf+2); // +2 - one for the extra / and one for \0
            if(stale_file == NULL){
                printf("Memory allocation error.\n");
                return -1;
            }
            // Construct full path to stale_file
        memcpy(stale_file_f, base_dir, base_dir_len);
        memcpy(stale_file_f + base_dir_len, stale_file, len_sf + 1);
        free(base_dir);
        printf("Full path to stale file: %s\n", stale_file_f);


        // Check and create a stale file to mark that the process is running.
        if(check_stale_file(stale_file_f)){
            printf("Stale file already exists\n");
                
        }else{
            printf("Stale file doesn't exist\n");
            stale_fileptr = fopen(stale_file_f, "w");
            if (stale_fileptr == NULL){
                printf("Stale file creation failed.\n");
                return -2;
            }else{
                printf("Stale file created.\n");
            }
        }

        
        fclose(stale_fileptr);
        free(stale_file_f);
        return 0;

    }

