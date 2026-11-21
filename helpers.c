#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include "helpers.h"

// Sanitize the file name.
// Eg. /data/local/data.txt --> _data_local_data_txt
char *sanitize(char *filename)
{
    size_t len = strlen(filename);
    char *sanitized_fname = malloc(len + 1);
    if (sanitized_fname == NULL)
    {
        return NULL;
    }
    memcpy(sanitized_fname, filename, len);

    const char UNIX_SUPPORTED_CHARS[33] = "!/\"#$%&'()*+,-.:;<=>?@[\\]^`{|}~";

    for (int j = 0; UNIX_SUPPORTED_CHARS[j] != '\0'; ++j)
    {
        // printf("%c\n", UNIX_SUPPORTED_CHARS[j]);
        for (int i = 0; sanitized_fname[i] != '\0'; i++)
        {
            if (UNIX_SUPPORTED_CHARS[j] == sanitized_fname[i])
                sanitized_fname[i] = '_';
        }
    }

    return sanitized_fname;
}

// Check if stale file already exists
bool check_stale_file(const char *fname)
{
    return (access(fname, F_OK) == 0) ? true : false;
}

// Construc the base directory for filewatcher and will always be ~/fwatcher/
// Memory needs to be freed.
char *get_base_dir()
{
    struct stat st = {0};

    const char *home_dir = getenv("HOME");
    const char fw_home[] = "/fwatcher/"; // Home for file watcher is always ~/fwatcher/ and the len of this string is 11 including \0.
    size_t home_len = strlen(home_dir);

    // Generate base dir
    char *base_dir = malloc(home_len + 11);
    if (base_dir == NULL)
    {
        return NULL;
    }
    memcpy(base_dir, home_dir, home_len);
    memcpy(base_dir + home_len, fw_home, 11);
    // Check if base dir already exist and create if it doesn't
    if (stat(base_dir, &st) == 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            return base_dir;
        }
        else
        {
            printf("Base path '%s' already exists but is not a directory.\n", base_dir);
            return NULL;
        }
    }
    if (mkdir(base_dir, 0755) == 0)
        printf("Base directory '%s' created.\n", base_dir);
    return base_dir;
}

bool wait_on_file(char *filename, u_int16_t wait_time, bool watch)
{
    bool file_exist_already = true; // Assume the file already exists
    struct stat st;
    time_t last_mod = 0;
    char buffer[26];
    // Formats as YYYY-MM-DD HH:MM:SS

    while (true)
    {
        if (access(filename, F_OK) == 0)
        {
            if (file_exist_already && watch)
            {
                if (stat(filename, &st) == 0)
                { // syscall for checking file metadata
                    last_mod = st.st_mtime;
                    struct tm *tm_info = localtime(&last_mod);
                    if (tm_info == NULL)
                    {
                        perror("localtime");
                        return false;
                    }
                    // Print out the last mod date and time
                    printf("File '%s' exists already with last mod %d-%d-%d %d:%d:%d \n",
                           filename,
                           tm_info->tm_year +1900,
                           tm_info->tm_mon,
                           tm_info->tm_mday,
                           tm_info->tm_hour,
                           tm_info->tm_min,
                           tm_info->tm_sec);

                    // Now wait for the file for modifications
                    while (true)
                    {
                        if (stat(filename, &st) == 0)
                        {
                            // Check if file was modified
                            if (st.st_mtime > last_mod)
                            {
                                struct tm *new_tm_info = localtime(&st.st_mtime);
                                // Print out the last mod date and time
                                printf("File '%s' has been modified at %d-%d-%d %d:%d:%d \n",
                                       filename,
                                       new_tm_info->tm_year + 1900,
                                       new_tm_info->tm_mon,
                                       new_tm_info->tm_mday,
                                       new_tm_info->tm_hour,
                                       new_tm_info->tm_min,
                                       new_tm_info->tm_sec);
                                return true;
                            }
                        }
                        sleep(wait_time);
                    }
                }
            }
            else
            {
                printf("File '%s' exists.\n", filename);
                return true;
            }
        }
        else
        {
            sleep(wait_time);
            printf("File '%s' does not exist.\n", filename);
            file_exist_already = false; // Confirms the file doesn't exist the first time around.
        }
    }
}
