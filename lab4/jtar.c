#include "tar_manager.h"

int main(int argc, char **argv)
{
    // If too few command line arguments are provided, a usage message is printed.
    if (argc < 2)
    {
        fprintf(stderr, "Usage: ./jtar [c | x][v] [file_or_dir_names]\n");
        return 0;
    }
    if (strcmp(argv[1], "c") == 0)
    {
        // Creates a TarManager object and adds all the files to it.
        TarManager *tar = create_tarmanager();
        for (int i = 2; i < argc; i++)
        {
            add(tar, argv[i]);
        }
        // Prints the tarfile to stdout.
        print_tar(tar, stdout);
        free_tarmanager(tar);
    }
    else if (strcmp(argv[1], "x") == 0)
    {
        // Reads in the tarfile from stdin and produces all files and directories.
        read_tar(stdin);
    }
}
