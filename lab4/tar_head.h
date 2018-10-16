#ifndef TAR_HEAD_H
#define TAR_HEAD_H

// These defines are here to enable the use of certain functions, like realpath, in C99.
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE

#include <features.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Copied from linux/limits.h
// Max path length
#define PATH_MAX 4096

// Compares two directories for equivalence
extern bool dir_eq(char *dir1, char *dir2);

// Macros defining file types
#define JTARNORMAL 0
// JTARSYMLINK should never be present in the tarfile. It is here for error checking purposes.
#define JTARSYMLINK 1
#define JTARDIR 2

// TarHeader stores the information for the header and links for a file in the tarfile.
typedef struct header_t
{ 
    // Full name of the file (includes path)
    char tar_name[PATH_MAX]; // offset = 0
    // Specifies if the entry is a file or directory
    uint8_t ftype; // offset = PATH_MAX
    // Stats for current file
    struct stat file_stats; // offset = PATH_MAX + 1
    // A checksum to confirm that the tarfile isn't corrupted
    int64_t checksum; // offset = PATH_MAX + 145
    // Note: hard_links and linknum are not directly part of the header
    // Stores the names of links to the same underlying data
    char **hard_links;
    // Number of links
    int linknum;
} TarHeader; // Copiable header size is PATH_MAX + 153

// Calculates the checksum from a TarHeader's underlying data.
// Note: hard_links not involved in the calculation
int64_t calc_checksum(TarHeader* thead);

// Basic wrapper struct around TarHeader to include local path and name for tarfile creation
typedef struct file_tarinfo_t
{
    char real_name[PATH_MAX];
    TarHeader *header_for_tar;
} FileInfo;

// Creates a FileInfo object for the provided file name and path.
extern FileInfo* create_header(char *fname, char *path_to_file);

// Recreates a TarHeader object from the header from a tarfile.
extern TarHeader* parse_header(char *head);

// Frees the memory used by the passed FileInfo object
extern void free_fileinfo(FileInfo *finfo);

// Frees the memory used by the passed TarHeader object
extern void free_tarheader(TarHeader *thead);

// Compares two TarHeader objects for equality
extern bool header_eq(TarHeader *h1, TarHeader *h2);

// Compares two stat structs for equality
extern bool stat_eq(struct stat *s1, struct stat *s2);

#endif
