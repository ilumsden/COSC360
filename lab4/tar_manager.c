#include <utime.h>

#include "tar_manager.h"

TarManager* create_tarmanager()
{
    TarManager *tar = (TarManager*) malloc(sizeof(TarManager));
    tar->num_files = 0;
    tar->headers = new_dllist();
    return tar;
}

void add(TarManager *tar, char *fname)
{
    struct stat buf;
    if ( lstat(fname, &buf) != 0 )
    {
        fprintf(stderr, "Error: could not stat %s\n", fname);
        exit(-1);
    }
    if (S_ISREG(buf.st_mode))
    {
        add_file(tar, fname);
    }
    else if (S_ISDIR(buf.st_mode))
    {
        add_dir(tar, fname);
    }
    return;
}

void add_file(TarManager *tar, char *fname)
{
    FileInfo *finfo = create_header(fname);
    Dllist ptr;
    Dllist nil = dll_nil(tar->headers);
    bool present = false;
    dll_traverse(ptr, tar->headers)
    {
        if (ptr == nil)
        {
            break;
        }
        FileInfo *curr = (FileInfo*) ptr->val.v;
        if (header_eq(finfo->header_for_tar, curr->header_for_tar) && (strcmp(finfo->real_name, curr->real_name) != 0))
        {
            present = true;
            break;
        }
        else if (stat_eq(&(finfo->header_for_tar.file_stats), &(curr->header_for_tar.file_stats)) &&
                 finfo->header_for_tar.ftype == JTARNORMAL)
        {
            strcpy(curr->header_for_tar.hard_links[curr->header_for_tar.linknum], finfo->header_for_tar.tar_name);
            curr->header_for_tar.linknum++;
            present = true;
            break;
        }
    }
    if (!present)
    {
        dll_append(tar->headers, new_jval_v(finfo));
        tar->num_files++;
    }
    else
    {
        free_fileinfo(finfo);
    }
}

void add_dir(TarManager *tar, char *dirname)
{
    DIR *currdir;
    struct dirent *currfile;
    currdir = opendir(dirname);
    if (currdir != NULL)
    {
        while ((currfile = readdir(currdir)) != NULL)
        {
            struct stat buf;
            if ( lstat(currfile->d_name, &buf) != 0 )
            {
                fprintf(stderr, "Error: could not stat %s\n", currfile->d_name);
                exit(-1);
            }
            if (S_ISDIR(buf.st_mode))
            {
                add_dir(tar, currfile->d_name);
            }
            else if (S_ISREG(buf.st_mode))
            {
                add_file(tar, currfile->d_name);
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: could not open directory %s\n", dirname);
        exit(-1);
    }
}

void print_tar(TarManager *tar, FILE *out)
{
    FileInfo *finfo;
    Dllist ptr;
    Dllist nil = dll_nil(tar->headers);
    dll_traverse(ptr, tar->headers)
    {
        if (ptr == nil)
        {
            break;
        }
        finfo = (FileInfo*) ptr->val.v;
        fwrite(finfo->header_for_tar.tar_name, 100, 1, out);
        fwrite(&(finfo->header_for_tar.ftype), sizeof(uint8_t), 1, out);
        fwrite(&(finfo->header_for_tar.file_stats), sizeof(struct stat), 1, out);
        fwrite(&(finfo->header_for_tar.checksum), sizeof(int64_t), 1, out);
        if (finfo->header_for_tar.file_stats.st_nlink > 1)
        {
            for (int i = 0; i < (int)(finfo->header_for_tar.file_stats.st_nlink-1); i++)
            {
                fwrite(finfo->header_for_tar.hard_links[i], 100, 1, out);
            }
        }
        FILE *currfile = fopen(finfo->real_name, "r");
        if (currfile == NULL)
        {
            fprintf(stderr, "Error: file could not be opened\n");
            exit(-1);
        }
        char *buf = (char*) malloc(finfo->header_for_tar.file_stats.st_size);
        fread(buf, finfo->header_for_tar.file_stats.st_size, 1, currfile);
        fwrite(buf, finfo->header_for_tar.file_stats.st_size, 1, out);
        fclose(currfile);
    }
}

void read_tar(char *fname)
{
    FILE *tarfile = fopen(fname, "r");
    if (tarfile == NULL)
    {
        fprintf(stderr, "Error: file could not be opened\n");
        exit(-1);
    }
    TarHeader *thead;
    char *header = (char*) malloc((100+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)));
    char *fdata;
    while (fread(header, (100+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)), 1, tarfile) == 1)
    {
        thead = parse_header(header);
        if (thead->file_stats.st_nlink > 1)
        {
            for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
            {
                if (fread(thead->hard_links[i], 100, 1, tarfile) != 1)
                {
                    if (feof(tarfile))
                    {
                        fprintf(stderr, "Error: EOF reached early.\n");
                    }
                    else
                    {
                        fprintf(stderr, "Error: could not read in the hard link name.\n");
                    }
                    exit(-1);
                }
            }
        }
        fdata = (char*) malloc(thead->file_stats.st_size);
        if (fread(fdata, thead->file_stats.st_size, 1, tarfile) != 1)
        {
            if (feof(tarfile))
            {
                fprintf(stderr, "Error: EOF reached early.\n");
            }
            else
            {
                fprintf(stderr, "Error: could not read in file data.\n");
            }
            exit(-1);
        }
        recreate_file(thead, fdata);
        free(fdata);
        free(header);
        header = (char*) malloc((100+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)));
    }
    free(header);
    if (!feof(tarfile))
    {
        fprintf(stderr, "Error: reading ended before end of file\n");
        exit(-1);
    }
}

void create_dir(char *dirname)
{
    struct stat buf;
    if ( lstat(dirname, &buf) != 0 )
    {
        if ( mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0 )
        {
            fprintf(stderr, "Error: Could not create directory %s\n", dirname);
            exit(-1);
        }
    }
}

char* create_subdirs(char *fname)
{
    char *new_dir = (char*) malloc(50);
    char *next_sub = (char*) malloc(50);
    new_dir[0] = 0;
    next_sub[0] = 0;
    new_dir = strtok(fname, "/");
    next_sub = strtok(NULL, "/");
    while (next_sub != NULL)
    {
        create_dir(new_dir);
        if ( chdir(new_dir) != 0 )
        {
            fprintf(stderr, "Error: Could not enter %s\n", new_dir);
            exit(-1);
        }
        strcpy(new_dir, next_sub);
        next_sub = strtok(NULL, "/");
    }
    return new_dir;
}

void recreate_file(TarHeader *thead, char *filedata)
{
    char *cwd = (char*) malloc(50);
    if ( getcwd(cwd, 50) == NULL )
    {
        fprintf(stderr, "Error: Could not get current working directory\n");
        exit(-1);
    }
    char *fname = create_subdirs(thead->tar_name);
    FILE *f = fopen(fname, "w");
    if (f == NULL)
    {
        fprintf(stderr, "Error: Could not create %s\n", thead->tar_name);
        exit(-1);
    }
    if (fwrite(filedata, thead->file_stats.st_size, 1, f) != 1)
    {
        fprintf(stderr, "Error: Could not write to %s\n", thead->tar_name);
        exit(-1);
    }
    fclose(f);
    if ( chmod(fname, thead->file_stats.st_mode) != 0 )
    {
        fprintf(stderr, "Error: Could not change modification data for %s\n", thead->tar_name);
        exit(-1);
    }
    struct utimbuf *times;
    times->actime = thead->file_stats.st_atime;
    times->modtime = thead->file_stats.st_mtime;
    if ( utime(fname, times) != 0 )
    {
        fprintf(stderr, "Error: Could not change time data for %s\n", thead->tar_name);
        exit(-1);
    }
    char absname[PATH_MAX];
    if ( realpath(fname, abspath) == NULL )
    {
        fprintf(stderr, "Error: Could not obtain the absolute path.\n");
        exit(-1);
    }
    if ( chdir(cwd) != 0 )
    {
        fprintf(stderr, "Error: Could not enter original directory\n");
        exit(-1);
    }
    if (thead->file_stats.st_nlink > 1)
    {
        for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
        {
            char *lname = create_subdirs(thead->hard_links[i]);
            if ( link(absname, lname) != 0 )
            {
                fprintf(stderr, "Error: Could not create link %s.\n", lname);
                exit(-1);
            }
            if ( chdir(cwd) != 0 )
            {
                fprintf(stderr, "Error: Could not enter original directory\n");
                exit(-1);
            }
        }
    }
}

void free_tarmanager(TarManager *tar)
{
    Dllist ptr;
    Dllist nil = dll_nil(tar->headers);
    dll_traverse(ptr, tar->headers)
    {
        FileInfo *finfo = (FileInfo*) ptr->val.v;
        free_fileinfo(finfo);
    }
    free_dllist(tar->headers);
    free(tar);
}
