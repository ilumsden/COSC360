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

extern TarManager* create_tarmanager();

extern void add(TarManager *tar, char *fname);

void add_file(TarManager *tar, char *fname);

void add_dir(TarManager *tar, char *dirname);

extern void print_tar(TarManager *tar, FILE *out);

extern void free_tarmanger(TarManager *tar);

extern void read_tar(char *fname);

void create_dir(char *dirname);

char* create_subdirs(char *fname);

void recreate_file(TarManager *tar, char *filedata);
