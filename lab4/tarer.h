#include "tar_head.h"
#include "dllist.h"

typedef struct tar_t
{
    int num_files;
    char *file_separator;
    char *tar_eof;
    Dllist headers;
} Tarer;

export Tarer* create_tarer();
