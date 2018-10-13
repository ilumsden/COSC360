#ifndef TAR_HEAD_H
#define TAR_HEAD_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <features.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern bool dir_eq(char *dir1, char *dir2);

char* remove_relative_specifiers_from_path(char *fname);

void remove_substring(char *s, const char *sub);

#define JTARNORMAL 0
#define JTARSYMLINK 1
#define JTARDIR 2

// Size is 253 bytes
typedef struct header_t
{ 
    // offset = 0
    char tar_name[100];
    // offset = 100
    uint8_t ftype;
    // offset = 101
    struct stat file_stats;
    // offset = 245
    int64_t checksum;
    char **hard_links;
    int linknum;
} TarHeader;

int64_t calc_checksum(TarHeader* thead);

typedef struct file_tarinfo_t
{
    char real_name[100];
    TarHeader *header_for_tar;
} FileInfo;

extern FileInfo* create_header(char *fname, char *path_to_file);

extern TarHeader* parse_header(char *head);

extern void free_fileinfo(FileInfo *finfo);

extern void free_tarheader(TarHeader *thead);

extern bool header_eq(TarHeader *h1, TarHeader *h2);

extern bool stat_eq(struct stat *s1, struct stat *s2);

#endif
