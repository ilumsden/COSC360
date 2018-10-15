#include "tar_manager.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: ./jtar [c | x][v] [file_or_dir_names]\n");
        return 0;
    }
    bool vflag = (strlen(argv[1]) == 2 && argv[1][1] == 'v');
    if (strcmp(argv[1], "c") == 0 || strcmp(argv[1], "cv") == 0)
    {
        TarManager *tar = create_tarmanager();
        for (int i = 2; i < argc; i++)
        {
            add(tar, argv[i]);
        }
        //reorder_headers(tar);
        print_tar(tar, stdout);
        free_tarmanager(tar);
    }
    else if (strcmp(argv[1], "x") == 0 || strcmp(argv[1], "xv") == 0)
    {
        read_tar(stdin);
    }
}
