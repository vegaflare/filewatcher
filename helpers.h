#ifndef HELPERS_H
#define HELPERS_H

char *sanitize(char *filename);
bool check_stale_file(const char *fname);
char *get_base_dir();
bool wait_on_file(char *filename, u_int16_t wait_time, bool watch);

#endif