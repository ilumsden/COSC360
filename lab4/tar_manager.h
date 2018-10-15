#ifndef TAR_MANAGER_H
#define TAR_MANAGER_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <limits.h>

#include "tar_head.h"
#include "jval.h"
#include "dllist.h"

typedef struct tar_t
{
    int num_files;
    Dllist headers;
} TarManager;

extern TarManager* create_tarmanager();

extern void add(TarManager *tar, char *fname);

void add_file(TarManager *tar, char *fname, char *appendpath);

void add_dir(TarManager *tar, char *dirname, char *appendpath, bool from_add_dir);

extern void reorder_headers(TarManager *tar);

extern void print_tar(TarManager *tar, FILE *out);

extern void free_tarmanager(TarManager *tar);

extern void read_tar(FILE *tarfile);

void create_dir(char *dirname);

char* create_subdirs(char *fname);

void recreate_file(TarHeader *thead, char *filedata);

extern void recreate_dir(TarHeader *thead);

#endif
