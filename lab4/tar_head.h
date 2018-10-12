#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern bool dir_eq(char *dir1, char *dir2);

char* remove_relative_specifiers_from_path(char *fname);

void remove_substring(char *s, const char *sub);

#define JTARNORMAL 0
#define JTARSYMLINK 1
#define JTARDIR 2

typedef struct header_t
{
    char tar_name[100]; // offset = 0
    uint8_t ftype; // offset = 100
    struct stat file_stats; // offset = 101
    int64_t checksum; // offset = 245
} TarHeader; // Size is 253 bytes

int64_t calc_checksum(TarHeader* thead);

typedef struct file_tarinfo_t
{
    char real_name[100];
    TarHeader *header_for_tar;
} FileInfo;

extern FileInfo* create_header(char *fname);

extern TarHeader* parse_header(char *head);

extern void free_fileinfo(FileInfo *finfo);

extern bool header_eq(TarHeader *h1, TarHeader *h2);
