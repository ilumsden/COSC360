#include "tar_head.h"
#include "dllist.h"

struct tar_t
{
    int num_files;
    char *file_separator;
    char *tar_eof;
    Dllist headers;
} Tarer;


