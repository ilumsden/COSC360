#include <dirent.h>
#include <sys/types.h>

#include "tar_head.h"
#include "jval.h"
#include "dllist.h"

typedef struct tar_t
{
    int num_files;
    Dllist headers;
} TarManager;

export TarManager* create_tarmanager();

export void add(TarManager *tar, char *fname);

void add_file(TarManager *tar, char *fname);

void add_dir(TarManager *tar, char *dirname);

export void print_tar(TarManager *tar, FILE *out);

export void free_tarmanger(TarManager *tar);

export void read_tar(char *fname);

void create_dir(char *dirname);

char* create_subdirs(char *fname);

void recreate_file(TarManager *tar, char *filedata);
