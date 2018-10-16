#include "tar_head.h"

bool dir_eq(char *dir1, char *dir2)
{
    // If the directory names are the same, return true.
    // This assumes that the path to the directories is contained in dir1 and dir2.
    if (strcmp(dir1, dir2) == 0)
    {
        return true;
    }
    struct stat buf1, buf2;
    // Print an error if the stats of either directory cannot be obtained.
    if ( lstat(dir1, &buf1) != 0 )
    {
        fprintf(stderr, "Error (dir_eq): Could not stat directory %s\n", dir1);
        exit(-1);
    }
    if ( lstat(dir2, &buf2) != 0 )
    {
        fprintf(stderr, "Error (dir_eq): Could not stat directory %s\n", dir2);
        exit(-1);
    }
    // Return true if the directories have the same Device ID and inode.
    // Return false otherwise.
    return (buf1.st_dev == buf2.st_dev) && (buf1.st_ino == buf2.st_ino);
}

int64_t calc_checksum(TarHeader *thead)
{
    int64_t sum = 0;
    // For all characters in the file name, add the integer value of the character to the sum.
    for (int i = 0; i < PATH_MAX; i++)
    {
        if (thead->tar_name[i] == 0)
        {
            break;
        }
        sum += (int64_t) thead->tar_name[i];
    }
    // For each other member (excluding link information and including each part of the stat struct),
    // add the integer value of the member to the sum.
    sum += thead->ftype;
    sum += (int) thead->file_stats.st_dev;
    sum += (int) thead->file_stats.st_ino;
    sum += (int) thead->file_stats.st_mode;
    sum += (int) thead->file_stats.st_nlink;
    sum += (int) thead->file_stats.st_uid;
    sum += (int) thead->file_stats.st_gid;
    sum += (int) thead->file_stats.st_rdev;
    sum += (int) thead->file_stats.st_size;
    sum += (int) thead->file_stats.st_blksize;
    sum += (int) thead->file_stats.st_blocks;
    sum += (int) thead->file_stats.st_blocks;
    sum += (int) thead->file_stats.st_atime;
    sum += (int) thead->file_stats.st_mtime;
    sum += (int) thead->file_stats.st_ctime;
    return sum;
}

FileInfo* create_header(char *fname, char *path_to_file)
{
    // Allocate and initialize all members of the FileInfo and TarHeader objects
    FileInfo *finfo = (FileInfo*) malloc(sizeof(FileInfo));
    TarHeader *thead = (TarHeader*) malloc(sizeof(TarHeader));
    finfo->real_name[0] = 0;
    thead->tar_name[0] = 0;
    thead->ftype = 4;
    memset(&(thead->file_stats), 0, sizeof(struct stat));
    thead->checksum = 0;
    thead->hard_links = (char**) malloc(sizeof(char*));
    thead->linknum = 0;
    // If path_to_file is not an empty string, combine path_to_file and fname to produce the full name, and
    // set the TarHeader tar_name and FileInfo real_name fields with this name.
    if (strcmp(path_to_file, "") != 0)
    {
        char *modname = (char*) malloc(strlen(path_to_file) + strlen(fname) + 2);
        modname[0] = 0;
        strcpy(modname, path_to_file);
        strncat(modname, "/", 1);
        strncat(modname, fname, strlen(fname));
        strcpy(finfo->real_name, modname);
        strcpy(thead->tar_name, modname);
	// This was added because my code occassionally added a / to fname for no reason.
        if (fname[0] == '/')
        {
            char *tmp = strdup(fname);
            strcpy(fname, tmp+1);
            free(tmp);
        }
        free(modname);
    }
    // If path_to_file is an empty string, copy fname into finfo->real_name and thead->tar_name
    else
    {
        strcpy(finfo->real_name, fname);
        strcpy(thead->tar_name, fname);
    }
    // Try to obtain file stats and store them into the TarHeader object.
    // Errors if the file's stats cannot be obtained.
    if ( lstat(fname, &(thead->file_stats)) != 0 )
    {
        fprintf(stderr, "Error (create_header): Could not stat %s\n", fname);
        exit(-1);
    }
    // Set the file type to the corresponding value.
    // If the file type doesn't match any accepted type, an error is raised.
    if (S_ISDIR(thead->file_stats.st_mode) != 0)
    {
        thead->ftype = JTARDIR;
    }
    else if (S_ISREG(thead->file_stats.st_mode) != 0)
    {
        thead->ftype = JTARNORMAL;
    }
    else if (S_ISLNK(thead->file_stats.st_mode) != 0)
    {
        thead->ftype = JTARSYMLINK;
    }
    else
    {
        fprintf(stderr, "Error: Could not determine file type of %s\n", fname);
        exit(-1);
    }
    // Calculate and set the file's checksum
    thead->checksum = calc_checksum(thead);
    // If the (normal) file has hard links, allocate space to store the links' names.
    if (thead->ftype == JTARNORMAL && thead->file_stats.st_nlink > 1)
    {
        free(thead->hard_links);
        thead->hard_links = (char**) malloc((thead->file_stats.st_nlink-1)*sizeof(char*));
        for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
        {
            thead->hard_links[i] = (char*) malloc(PATH_MAX);
            thead->hard_links[i][0] = 0;
        }
    }
    // Otherwise, allocate the bare minimum space for the hard_links array to prevent compilation errors.
    else
    {
        free(thead->hard_links);
        thead->hard_links = (char**) malloc(sizeof(char*));
        thead->hard_links[0] = (char*) malloc(1);
        thead->hard_links[0][0] = 0;
    }
    // Connect the TarHeader object to the FileInfo object
    finfo->header_for_tar = thead;
    return finfo;
}

TarHeader* parse_header(char *head)
{
    TarHeader *thead = (TarHeader*) malloc(sizeof(TarHeader));
    // Copies data from the header buffer into the TarHeader object
    memcpy(thead->tar_name, &head[0], PATH_MAX);
    memcpy(&thead->ftype, &head[PATH_MAX], 1);
    memcpy(&thead->file_stats, &head[PATH_MAX+1], 144);
    memcpy(&thead->checksum, &head[PATH_MAX+145], 8);
    // Calculate a checksum based on the data extracted from the tarfile header.
    int64_t calc_sum = calc_checksum(thead);
    // Raise an error if the calculated checksum doesn't match the one obtained from the tarfile.
    // This suggests the tarfile is corrupted.
    if (thead->checksum != calc_sum)
    {
        fprintf(stderr, "Error: Tarfile corrupted! Checksums don't match.\n");
        exit(-1);
    }
    // Allocate space for links in the same way as create_header
    if (thead->file_stats.st_nlink > 1)
    {
        thead->hard_links = (char**) malloc((thead->file_stats.st_nlink-1)*sizeof(char*));
        for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
        {
            thead->hard_links[i] = (char*) malloc(PATH_MAX);
            thead->hard_links[i][0] = 0;
        }
    }
    else
    {
        thead->hard_links = (char**) malloc(sizeof(char*));
        thead->hard_links[0] = (char*) malloc(1);
        thead->hard_links[0][0] = 0;
    }
    return thead;
}

void free_fileinfo(FileInfo *finfo)
{
    free_tarheader(finfo->header_for_tar);
    free(finfo);
}

void free_tarheader(TarHeader *thead)
{
    if (thead->file_stats.st_nlink > 1 && thead->ftype == JTARNORMAL)
    {
        for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
        {
            free(thead->hard_links[i]);
        }
    }
    else
    {
        free(thead->hard_links[0]);
    }
    free(thead->hard_links);
    free(thead);
}

bool header_eq(TarHeader *h1, TarHeader *h2)
{
    // Return false if the tar_name fields are not the same.
    if (strcmp(h1->tar_name, h2->tar_name) != 0)
    {
        return false;
    }
    // Return true if all details about the files/directories are the same.
    return (h1->ftype == h2->ftype) && stat_eq(&(h1->file_stats), &(h2->file_stats));
}

bool stat_eq(struct stat *s1, struct stat *s2)
{
    return (s1->st_dev == s2->st_dev) &&
           (s1->st_ino == s2->st_ino) && (s1->st_mode == s2->st_mode) &&
           (s1->st_nlink == s2->st_nlink) && (s1->st_uid == s2->st_uid) &&
           (s1->st_gid == s2->st_gid) && (s1->st_rdev == s2->st_rdev) &&
           (s1->st_size == s2->st_size) && (s1->st_blksize == s2->st_blksize) &&
           (s1->st_blocks == s2->st_blocks) && (s1->st_atime == s2->st_atime) &&
           (s1->st_mtime == s2->st_mtime) && (s1->st_ctime == s2->st_ctime);
}
