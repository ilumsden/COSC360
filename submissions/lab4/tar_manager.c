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
    // Obtains stats for the file. Raises an error if it can't successfuilly stat the file/directory.
    struct stat buf;
    if ( lstat(fname, &buf) != 0 )
    {
        fprintf(stderr, "Error: could not stat %s\n", fname);
        exit(-1);
    }
    // Call add_file if fname is a file.
    if (S_ISREG(buf.st_mode))
    {
        add_file(tar, fname, "");
    }
    // Call add_dir if fname is a directory.
    else if (S_ISDIR(buf.st_mode))
    {
        add_dir(tar, fname, "", false);
    }
    return;
}

void add_file(TarManager *tar, char *fname, char *appendpath)
{
    // This first part up until creating the FileInfo object is only run if fname contains the path to the file.
    char *new_dir, *next_sub;
    char *app = (char*) malloc(PATH_MAX);
    app[0] = 0;
    bool first = true;
    // If fname is contains a path, obtain the first directory name and whatever directory or file name comes after it.
    // If fname is just a file, new_dir will be the same as fname, and next_sub will be NULL.
    new_dir = strtok(fname, "/");
    next_sub = strtok(NULL, "/");
    while (next_sub != NULL)
    {
        // Call add_dir with from_add_file set to true for the directory name in new_dir.
        add_dir(tar, new_dir, app, true);
        // Add new_dir to app, which is the path to the text directory.
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
        // Copy the value of next_sub into new_dir and grab the next file/directory name from fname.
        strcpy(new_dir, next_sub);
        next_sub = strtok(NULL, "/");
    }
    free(app);
    // Creates the header for the file.
    FileInfo *finfo;
    finfo = create_header(fname, appendpath);
    Dllist ptr;
    Dllist nil = dll_nil(tar->headers);
    bool present = false;
    // If the header is already in the TarManager's Dllist under the exact same name, just free the TarHeader.
    // If the header representing a file is already in the TarManager's Dllist under a different name, add the header's
    // tar_name field to the hard_links array in the pre-existing header.
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

void add_dir(TarManager *tar, char *dirname, char *appendpath, bool from_add_file)
{
    // Get current working directory
    char *cwd = (char*) malloc(1024);
    cwd[0] = 0;
    if ( getcwd(cwd, 1024) == NULL )
    {
        fprintf(stderr, "Error: Could not get current working directory\n");
        exit(-1);
    }
    Dllist dirs = new_dllist();
    DIR *currdir;
    struct dirent *currfile;
    // Open the directory dirname
    currdir = opendir(dirname);
    if (currdir != NULL)
    {
        // If this function is called from add_file, call add_file for dirname.
        if (from_add_file)
        {
            add_file(tar, dirname, appendpath);
            closedir(currdir);
            return;
        }
        // Create appath so that it stores the concatentation of appendpath and dirname.
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
        // Move into the directory dirname
        if ( chdir(dirname) != 0 )
        {
            fprintf(stderr, "Error: could not move to directory %s\n", dirname);
            exit(-1);
        }
        // Get the names of every file and directory in dirname
        while ((currfile = readdir(currdir)) != NULL)
        {
            if (strcmp(currfile->d_name, ".") == 0 || strcmp(currfile->d_name, "..") == 0)
            {
                continue;
            }
            else
            {
                dll_append(dirs, new_jval_s(strdup(currfile->d_name)));
            }
        }
        // Close the directory to decrease the number of open files.
        closedir(currdir);
        // For each file or directory in dirname, call add_file to add it to TarManager.
        // If the current element is a directory, also recursively call add_dir on it.
        Dllist ptr;
        Dllist dir_nil = dll_nil(dirs);
        dll_traverse(ptr, dirs)
        {
            if (ptr == dir_nil)
            {
                break;
            }
            char *currfname = ptr->val.s;
            struct stat buf;
            if ( lstat(currfname, &buf) != 0 )
            {
                fprintf(stderr, "Error: could not stat %s\n", currfname);
                exit(-1);
            }
            if (S_ISDIR(buf.st_mode))
            {
                add_file(tar, currfname, appath);
                add_dir(tar, currfname, appath, false);
            }
            else if (S_ISREG(buf.st_mode))
            {
                add_file(tar, currfname, appath);
            }
            free(currfname);
        }
        free_dllist(dirs);
        free(appath);
    }
    else
    {
        fprintf(stderr, "Error: could not open directory %s/%s\n", appendpath, dirname);
        closedir(currdir);
        exit(-1);
    }
    // Move back to the directory that the function started in.
    if ( chdir(cwd) != 0 )
    {
        fprintf(stderr, "Error: could not move to previous directory %s\n", dirname);
        exit(-1);
    }
    free(cwd);
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
        // Write the header data to the tarfile.
        finfo = (FileInfo*) ptr->val.v;
        fwrite(finfo->header_for_tar->tar_name, PATH_MAX, 1, out);
        fwrite(&(finfo->header_for_tar->ftype), sizeof(uint8_t), 1, out);
        fwrite(&(finfo->header_for_tar->file_stats), sizeof(struct stat), 1, out);
        fwrite(&(finfo->header_for_tar->checksum), sizeof(int64_t), 1, out);
        // If the current header represents a file that has links, write the names of the links to the tarfile.
        if (finfo->header_for_tar->ftype == JTARNORMAL && 
            finfo->header_for_tar->file_stats.st_nlink > 1)
        {
            for (int i = 0; i < (int)(finfo->header_for_tar->file_stats.st_nlink-1); i++)
            {
                fwrite(finfo->header_for_tar->hard_links[i], PATH_MAX, 1, out);
            }
        }
        // If the current header represents a file, read the file's contents and write them to the tarfile.
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
    // While loop is controlled by successfully reading in a header from the tarfile.
    while (fread(header, (PATH_MAX+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)), 1, tarfile) == 1)
    {
        // Creates a TarHeader object from the buffer containing the header's data.
        thead = parse_header(header);
        if (thead->ftype == JTARNORMAL)
        {
            // If the TarHeader object represents a file that has links, the link names are read from the tarfile.
            if (thead->file_stats.st_nlink > 1)
            {
                for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
                {
                    if (fread(thead->hard_links[i], PATH_MAX, 1, tarfile) != 1)
                    {
                        // Raises an error if the read failed.
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
            // Reads the file's contents from the tarfile into a buffer.
            fdata = (char*) malloc(thead->file_stats.st_size);
            if (fread(fdata, thead->file_stats.st_size, 1, tarfile) != 1)
            {
                // Raises an error if the read failed.
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
            // Recreates the file from the TarHeader object and the file's contents.
            recreate_file(thead, fdata);
            free(fdata);
            free_tarheader(thead);
        }
        // If the header represents a directory, add the header to the back of an array.
        else if (thead->ftype == JTARDIR)
        {
            dirs[num_dirs] = thead;
            num_dirs++;
        }
        free(header);
        header = (char*) malloc((PATH_MAX+sizeof(uint8_t)+sizeof(struct stat)+sizeof(int64_t)));
    }
    free(header);
    // If the file stream is not at end-of-file when the reading is complete, raise an error.
    if (!feof(tarfile))
    {
        fprintf(stderr, "Error: reading ended before end of file\n");
        exit(-1);
    }
    // Traverses backwards through the header array, and calls recreate_dir on each header.
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
    // If the directory doesn't already exist, create it with default permissions.
    // If the creation fails, raise an error.
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
    // Loops through each directory name in fname (based on the / character).
    // For each directory, create_dir is called, and then the code moves into that directory.
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
    // Returns the file name without the path. Everything above this is effectively skipped if
    // a file without a path is passed.
    return new_dir;
}

void recreate_file(TarHeader *thead, char *filedata)
{
    // Gets current working directory.
    char *cwd = (char*) malloc(1024);
    cwd[0] = 0;
    if ( getcwd(cwd, 1024) == NULL )
    {
        fprintf(stderr, "Error: Could not get current working directory\n");
        exit(-1);
    }
    // Creates the subdirs associated with the files.
    char *fname = create_subdirs(thead->tar_name);
    // Creates and opens the file.
    FILE *f = fopen(fname, "w");
    // Raises an error if the open failed.
    if (f == NULL)
    {
        fprintf(stderr, "Error: Could not create %s\n", thead->tar_name);
        exit(-1);
    }
    // Writes the file's contents into the file. Raises an error if the write fails.
    if (fwrite(filedata, thead->file_stats.st_size, 1, f) != 1)
    {
        fprintf(stderr, "Error: Could not write to %s\n", thead->tar_name);
        exit(-1);
    }
    fclose(f);
    // Changes the file's permissions to the permissions stored in the TarHeader object.
    // Raises an error if the permission change fails.
    if ( chmod(fname, thead->file_stats.st_mode) != 0 )
    {
        fprintf(stderr, "Error (recreate_file): Could not change modification data for %s\n", thead->tar_name);
        exit(-1);
    }
    // Changes the file's access and modification times to those stored in the TarHeader object.
    // Raises an error if the time change fails.
    struct utimbuf times;
    times.actime = thead->file_stats.st_atime;
    times.modtime = thead->file_stats.st_mtime;
    if ( utime(fname, &times) != 0 )
    {
        fprintf(stderr, "Error: Could not change time data for %s\n", thead->tar_name);
        exit(-1);
    }
    // Gets the absolute path to the file.
    char *absname = realpath(fname, NULL);
    if ( absname == NULL )
    {
        fprintf(stderr, "Error: Could not obtain the absolute path.\n");
        exit(-1);
    }
    // Returns to the starting directory. Raises an error if the directory change fails.
    if ( chdir(cwd) != 0 )
    {
        fprintf(stderr, "Error: Could not enter original directory\n");
        exit(-1);
    }
    // If the file has links, creates them and any directories needed to create the link.
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
    // Gets current working directory.
    char *cwd = (char*) malloc(1024);
    cwd[0] = 0;
    if ( getcwd(cwd, 1024) == NULL )
    {
        fprintf(stderr, "Error: Could not get current working directory\n");
        exit(-1);
    }
    // Sees if the directory exists. If it doesn't, creates all levels of parent directories
    // that don't exist (uses create_subdirs).
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
    // Changes the directory's permissions to the permissions stored in the TarHeader object.
    // Raises an error if the permission change fails.
    if ( chmod(thead->tar_name, thead->file_stats.st_mode) != 0 )
    {
        fprintf(stderr, "Error (recreate_dir): Could not change modification data for %s\n", thead->tar_name);
        exit(-1);
    }
    // Changes the directory's access and modification times to those stored in the TarHeader object.
    // Raises an error if the time change fails.
    struct utimbuf times;
    times.actime = thead->file_stats.st_atime;
    times.modtime = thead->file_stats.st_mtime;
    if ( utime(thead->tar_name, &times) != 0 )
    {
        fprintf(stderr, "Error: Could not change time data for %s\n", thead->tar_name);
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
