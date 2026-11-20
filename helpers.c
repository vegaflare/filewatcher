#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include "helpers.h"

// Sanitize the file name.
// Eg. /data/local/data.txt --> _data_local_data_txt
char *sanitize(char *filename){
    
    const char UNIX_SUPPORTED_CHARS[33] = "!/\"#$%&'()*+,-.:;<=>?@[\\]^`{|}~";

    for (int j =0;UNIX_SUPPORTED_CHARS[j]!= '\0'; ++j){
        //printf("%c\n", UNIX_SUPPORTED_CHARS[j]);
        for (int i=0;filename[i]!='\0';i++){
            if(UNIX_SUPPORTED_CHARS[j]==filename[i])
                filename[i] = '_';
        }
    }
    
    return filename;
}

// Check if stale file already exists
bool check_stale_file(const char *fname){
    return (access(fname, F_OK)==0)?  true : false;
}


// Construc the base directory for filewatcher and will always be ~/fwatcher/
// Memory needs to be freed.
char *get_base_dir(){
    struct stat st = {0};
    
    const char *home_dir = getenv("HOME");
    const char fw_home[] = "/fwatcher/"; //Home for file watcher is always ~/fwatcher/ and the len of this string is 11 including \0.
    size_t home_len = strlen(home_dir);

    // Generate base dir
    char *base_dir = malloc(home_len+ 11);
    if (base_dir == NULL){
        return NULL;
    }
    memcpy(base_dir, home_dir, home_len);
    memcpy(base_dir+home_len, fw_home, 11);
    // Check if base dir already exist and create if it doesn't
    if(stat(base_dir, &st) ==0 ){
        if(S_ISDIR(st.st_mode)){
            return base_dir;
        }else{
            printf("Base path '%s' already exists but is not a directory.\n", base_dir);
            return NULL;
        }
    }
    if(mkdir(base_dir, 0755) == 0)
        printf("Base directory '%s' created.\n", base_dir);
    return base_dir;
}

