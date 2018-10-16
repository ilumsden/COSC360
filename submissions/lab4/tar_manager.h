#ifndef TAR_MANAGER_H
#define TAR_MANAGER_H

// Defines these again to be safe
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <limits.h>

#include "tar_head.h"
#include "jval.h"
#include "dllist.h"

// This struct stores all header information needed to create the tarfile
typedef struct tar_t
{
    int num_files;
    Dllist headers;
} TarManager;

// Allocate memory and initialize a TarManager object.
extern TarManager* create_tarmanager();

// Function used for adding any file or directory into the TarManager object.
// Wraps add_file and add_dir
extern void add(TarManager *tar, char *fname);

// Adds a file or directory to the TarManager object.
void add_file(TarManager *tar, char *fname, char *appendpath);

// Adds the contents of the provided directory to the TarManager object.
void add_dir(TarManager *tar, char *dirname, char *appendpath, bool from_add_dir);

// Creates a tarfile by printing the contents of TarManager to the filestream out.
extern void print_tar(TarManager *tar, FILE *out);

// Frees the memory associated with the passed TarManager object.
extern void free_tarmanager(TarManager *tar);

// Recreates the files specified by the tarfile associated with the passed filestream.
extern void read_tar(FILE *tarfile);

// Creates a new directory if it doesn't exist
void create_dir(char *dirname);

// Creates all directories along the path inside fname and returns the file name by itself
char* create_subdirs(char *fname);

// Recreates a file based on the TarHeader object and the file's contnents.
void recreate_file(TarHeader *thead, char *filedata);

// Recreates a directory based on the TarHeader object.
extern void recreate_dir(TarHeader *thead);

#endif
