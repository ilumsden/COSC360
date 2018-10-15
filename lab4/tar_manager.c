#include "tar_manager.h"

#include <stdlib.h>
#include <utime.h>

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
        add_file(tar, fname, "");
    }
    else if (S_ISDIR(buf.st_mode))
    {
        add_dir(tar, fname, "", false);
    }
    return;
}

void add_file(TarManager *tar, char *fname, char *appendpath)
{
    char *new_dir, *next_sub;
    char *app = (char*) malloc(PATH_MAX);
    app[0] = 0;
    bool first = true;
    new_dir = strtok(fname, "/");
    next_sub = strtok(NULL, "/");
    while (next_sub != NULL)
    {
        add_dir(tar, new_dir, app, true);
        if (first)
        {
            strcpy(app, new_dir);
            first = false;
        }
        else
        {
            strcat(app, "/");
            strcat(app, new_dir);
        }
        strcpy(new_dir, next_sub);
        next_sub = strtok(NULL, "/");
    }
    free(app);
    FileInfo *finfo;
    finfo = create_header(fname, appendpath);
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
        if (curr == NULL)
        {
            break;
        }
        if (header_eq(finfo->header_for_tar, curr->header_for_tar) && (strcmp(finfo->real_name, curr->real_name) == 0))
        {
            present = true;
            break;
        }
        else if (stat_eq(&(finfo->header_for_tar->file_stats), &(curr->header_for_tar->file_stats)) &&
                 finfo->header_for_tar->ftype == JTARNORMAL)
        {
            strcpy(curr->header_for_tar->hard_links[curr->header_for_tar->linknum], finfo->header_for_tar->tar_name);
            curr->header_for_tar->linknum++;
            present = true;
            break;
        }
    }
    if (!present)
    {
        dll_append(tar->headers, new_jval_v((void*)finfo));
        tar->num_files++;
    }
    else
    {
        free_fileinfo(finfo);
    }
}

// TODO: Find way to prepend path to files
void add_dir(TarManager *tar, char *dirname, char *appendpath, bool from_add_file)
{
    char *cwd = (char*) malloc(1024);
    cwd[0] = 0;
    if ( getcwd(cwd, 1024) == NULL )
    {
        //fprintf(stderr, "Error: Could not get current working directory\n");
        perror("Error: Could not get current working directory\n");
        exit(-1);
    }
    DIR *currdir;
    struct dirent *currfile;
    currdir = opendir(dirname);
    if (currdir != NULL)
    {
        if (from_add_file)
        {
            add_file(tar, dirname, appendpath);
            closedir(currdir);
            return;
        }
        char *appath = (char*) malloc(strlen(appendpath) + strlen(dirname) + 2);
        appath[0] = 0;
        if (strcmp(appendpath, "") == 0)
        {
            strcpy(appath, dirname);
        }
        else
        {
            strcpy(appath, appendpath);
            strcat(appath, "/");
            strcat(appath, dirname);
        }
        if ( chdir(dirname) != 0 )
        {
            fprintf(stderr, "Error: could not move to directory %s\n", dirname);
            exit(-1);
        }
        // Update to close directories ASAP
        while ((currfile = readdir(currdir)) != NULL)
        {
            if (strcmp(currfile->d_name, ".") == 0 || strcmp(currfile->d_name, "..") == 0)
            {
                continue;
            }
            struct stat buf;
            if ( lstat(currfile->d_name, &buf) != 0 )
            {
                fprintf(stderr, "Error: could not stat %s\n", currfile->d_name);
                exit(-1);
            }
            if (S_ISDIR(buf.st_mode))
            {
                add_file(tar, currfile->d_name, appath);
                add_dir(tar, currfile->d_name, appath, false);
            }
            else if (S_ISREG(buf.st_mode))
            {
                add_file(tar, currfile->d_name, appath);
            }
        }
        free(appath);
    }
    else
    {
        //fprintf(stderr, "Error: could not open directory %s/%s\n", appendpath, dirname);
        perror("Error: could not open directory\n");
        closedir(currdir);
        exit(-1);
    }
    //closedir(currdir);
    //free(realdir);
    if ( chdir(cwd) != 0 )
    {
        fprintf(stderr, "Error: could not move to previous directory %s\n", dirname);
        exit(-1);
    }
    free(cwd);
}

void reorder_headers(TarManager *tar)
{
    Dllist reordered = new_dllist();
    Dllist dirs = new_dllist();
    Dllist files = new_dllist();
    Dllist ptr;
    Dllist head_nil = dll_nil(tar->headers);
    dll_traverse(ptr, tar->headers)
    {
        if (ptr == head_nil)
        {
            break;
        }
        TarHeader *thead = (TarHeader*) ptr->val.v;
        fprintf(stderr, "thead is %p\n", thead);
        if (thead->ftype == JTARDIR)
        {
            dll_append(dirs, new_jval_v((void*)thead));
        }
        else if (thead->ftype == JTARNORMAL)
        {
            dll_append(files, new_jval_v((void*)thead));
        }
    }
    free_dllist(tar->headers);
    dll_traverse(ptr, dirs)
    {
        TarHeader *thead = (TarHeader*) ptr->val.v;
        dll_append(reordered, new_jval_v((void*)thead));
    }
    free_dllist(dirs);
    dll_traverse(ptr, files)
    {
        TarHeader *thead = (TarHeader*) ptr->val.v;
        dll_append(reordered, new_jval_v((void*)thead));
    }
    free_dllist(files);
    tar->headers = reordered;
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
        fwrite(finfo->header_for_tar->tar_name, PATH_MAX, 1, out);
        fwrite(&(finfo->header_for_tar->ftype), sizeof(uint8_t), 1, out);
        fwrite(&(finfo->header_for_tar->file_stats), sizeof(struct stat), 1, out);
        fwrite(&(finfo->header_for_tar->checksum), sizeof(int64_t), 1, out);
        if (finfo->header_for_tar->ftype == JTARNORMAL && 
            finfo->header_for_tar->file_stats.st_nlink > 1)
        {
            for (int i = 0; i < (int)(finfo->header_for_tar->file_stats.st_nlink-1); i++)
            {
                fwrite(finfo->header_for_tar->hard_links[i], PATH_MAX, 1, out);
            }
        }
        if (finfo->header_for_tar->ftype == JTARNORMAL)
        {
            FILE *currfile = fopen(finfo->real_name, "r");
            if (currfile == NULL)
            {
                fprintf(stderr, "Error: file %s could not be opened\n", finfo->real_name);
                exit(-1);
            }
            char *buf = (char*) malloc(finfo->header_for_tar->file_stats.st_size);
            fread(buf, finfo->header_for_tar->file_stats.st_size, 1, currfile);
            fwrite(buf, finfo->header_for_tar->file_stats.st_size, 1, out);
            free(buf);
            fclose(currfile);
        }
    }
}

void read_tar(FILE *tarfile)
{
    TarHeader *thead;
    char *header = (char*) malloc((PATH_MAX+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)));
    char *fdata;
    TarHeader* dirs[100];
    int num_dirs = 0;
    while (fread(header, (PATH_MAX+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)), 1, tarfile) == 1)
    {
        thead = parse_header(header);
        if (thead->ftype == JTARNORMAL)
        {
            if (thead->file_stats.st_nlink > 1)
            {
                for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
                {
                    if (fread(thead->hard_links[i], PATH_MAX, 1, tarfile) != 1)
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
            free_tarheader(thead);
        }
        else if (thead->ftype == JTARDIR)
        {
            dirs[num_dirs] = thead;
            num_dirs++;
            //recreate_dir(thead);
        }
        free(header);
        header = (char*) malloc((PATH_MAX+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)));
    }
    free(header);
    if (!feof(tarfile))
    {
        fprintf(stderr, "Error: reading ended before end of file\n");
        exit(-1);
    }
    for (int i = num_dirs-1; i >= 0; i--)
    {
        TarHeader *th = dirs[i];
        recreate_dir(th);
        free_tarheader(th);
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
    char *new_dir, *next_sub;
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
    free(next_sub);
    return new_dir;
}

void recreate_file(TarHeader *thead, char *filedata)
{
    char *cwd = (char*) malloc(1024);
    cwd[0] = 0;
    if ( getcwd(cwd, 1024) == NULL )
    {
        //fprintf(stderr, "Error: Could not get current working directory\n");
        perror("Error: Could not get current working directory\n");
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
        fprintf(stderr, "Error (recreate_file): Could not change modification data for %s\n", thead->tar_name);
        exit(-1);
    }
    struct utimbuf times;
    times.actime = thead->file_stats.st_atime;
    times.modtime = thead->file_stats.st_mtime;
    if ( utime(fname, &times) != 0 )
    {
        fprintf(stderr, "Error: Could not change time data for %s\n", thead->tar_name);
        exit(-1);
    }
    char *absname = realpath(fname, NULL);
    if ( absname == NULL )
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
    free(cwd);
    free(absname);
}

void recreate_dir(TarHeader *thead)
{
    char *cwd = (char*) malloc(1024);
    cwd[0] = 0;
    if ( getcwd(cwd, 1024) == NULL )
    {
        //fprintf(stderr, "Error: Could not get current working directory\n");
        perror("Error: Could not get current working directory\n");
        exit(-1);
    }
    struct stat buf;
    if ( lstat(thead->tar_name, &buf) != 0 )
    {
        char *tmp = strdup(thead->tar_name);
        char *deepest_dir = create_subdirs(tmp);
        create_dir(deepest_dir);
        free(tmp);
        if ( chdir(cwd) != 0 )
        {
            fprintf(stderr, "Error: Could not enter original directory\n");
            exit(-1);
        }
    }
    free(cwd);
    if ( chmod(thead->tar_name, thead->file_stats.st_mode) != 0 )
    {
        fprintf(stderr, "Error (recreate_dir): Could not change modification data for %s\n", thead->tar_name);
        exit(-1);
    }
    struct utimbuf times;
    times.actime = thead->file_stats.st_atime;
    times.modtime = thead->file_stats.st_mtime;
    if ( utime(thead->tar_name, &times) != 0 )
    {
        fprintf(stderr, "Error: Could not change time data for %s\n", thead->tar_name);
        exit(-1);
    }
    struct stat buf1;
    if ( lstat(thead->tar_name, &buf) != 0 )
    {
        fprintf(stderr, "Error");
        exit(-1);
    }
}

void free_tarmanager(TarManager *tar)
{
    Dllist ptr;
    Dllist nil = dll_nil(tar->headers);
    dll_traverse(ptr, tar->headers)
    {
        if (ptr == nil)
        {
            break;
        }
        FileInfo *finfo = (FileInfo*) ptr->val.v;
        free_fileinfo(finfo);
    }
    free_dllist(tar->headers);
    free(tar);
}
