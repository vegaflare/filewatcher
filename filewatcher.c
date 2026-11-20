#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/file.h>
#include<sys/errno.h>
#include<time.h>
#include "helpers.h"



    int main(int argc, char *argv[]){
        bool watch_flag = false;
        int stale_fd;
        char *file_name = NULL;
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
                    if (file_pos >= argc){
                        printf("No file name passed after the flag '-f'.\n");
                        return 5;
                    }
                    size_t fname_len = strlen(argv[i+1]) + 1;
                    file_name = malloc(fname_len);
                    memcpy(file_name, argv[i+1], fname_len);
                    ++i;
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
            }else if ( file_name == NULL){
                printf("Failed to allocate memory for filename. Code -3\n");
                perror("malloc");
                return -3;
            }

            char *stale_file = sanitize(file_name);
            if (stale_file == NULL) {return 5;}
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
            char *stale_file_f = malloc(base_dir_len+len_sf+1); // one for \0
            if(stale_file_f == NULL){
                printf("Memory allocation error.\n");
                return -1;
            }
            // Construct full path to stale_file
        memcpy(stale_file_f, base_dir, base_dir_len);
        memcpy(stale_file_f + base_dir_len, stale_file, len_sf + 1);
        free(base_dir);
        printf("Full path to stale file: %s\n", stale_file_f);


        // Check and create a stale file to mark that the process is running.
        if(access(stale_file_f, F_OK)==0){
            printf("Stale file already exists.\n Opening file...\n");


            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);

            stale_fd = open(stale_file_f, O_RDWR);

            clock_gettime(CLOCK_MONOTONIC, &end);

            double elapsed =
                (end.tv_sec - start.tv_sec) +
                (end.tv_nsec - start.tv_nsec) / 1000000000.0;

            printf("open() took %.6f seconds\n", elapsed);



            printf("Not stuck\n");
            if(stale_fd == -1){
                
                printf("Failed to open file, %i.\n", errno);
                return -2;
            }
            if(flock(stale_fd, LOCK_EX | LOCK_NB) == -1){
                perror("flock");
                close(stale_fd);
                return -2;
            }
            printf("Stale_file_opened and locked: %i. \n", stale_fd);
            
        }else{
            printf("Stale file doesn't exist\n");
            stale_fd = open(stale_file_f, O_CREAT|O_RDWR|O_EXCL, 0644);
            if (stale_fd == -1){
                printf("Stale file creation failed.\n");
                return -2;
            }else{
                printf("Stale file created.\n");
            }
        }

        //Now wait for the file
        if(wait_on_file(file_name, 10)){
            printf("File found. Exiting...\n");
        }
        
        close(stale_fd);
        free(file_name);
        free(stale_file_f);
        return 0;

    }

