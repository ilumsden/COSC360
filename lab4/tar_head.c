#include "tar_head.h"

bool dir_eq(char *dir1, char *dir2)
{
    if (strcmp(dir1, dir2) == 0)
    {
        return true;
    }
    struct stat buf1, buf2;
    if ( lstat(dir1, &buf1) != 0 )
    {
        fprintf(stderr, "Error: Could not stat directory %s\n", dir1);
        exit(-1);
    }
    if ( lstat(dir2, &buf2) != 0 )
    {
        fprintf(stderr, "Error: Could not stat directory %s\n", dir2);
        exit(-1);
    }
    return (buf1.st_dev == buf2.st_dev) && (buf1.st_ino == buf2.st_ino);
}

char* remove_relative_specifiers_from_path(char *fname)
{
    char *norel = strdup(fname);
    remove_substring(norel, "../");
    remove_substring(norel, "./");
}

void remove_substring(char *s, const char *sub)
{
    int slen = strlen(s);
    int sublen = strlen(sub);
    bool found;
    for (int i = 0; i <= slen - sublen; i++)
    {
        found = true;
        for (int j = 0; j < sublen; j++)
        {
            if (s[i+j] != sub[j])
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            for (int j=i; j <= slen - sublen; j++)
            {
                s[j] = s[j + sublen];
            }
            slen = slen - sublen;
            i--;
        }
    }
    for (int i = slen; i < (int) strlen(s); i++)
    {
        s[i] = 0;
    }
}

int64_t calc_checksum(TarHeader *thead)
{
    int64_t sum = 0;
    for (int i = 0; i < 100; i++)
    {
        sum += 0xFF & thead->tar_name[i];
    }
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

FileInfo* create_header(char *fname)
{
    FileInfo *finfo = (FileInfo*) malloc(sizeof(FileInfo));
    TarHeader *thead = (TarHeader*) malloc(sizeof(TarHeader));
    strcpy(finfo->real_name, fname);
    char* tmp_str = remove_relative_specifiers_from_path(fname);
    strcpy(thead->tar_name, tmp_str);
    free(tmp_str);
    if ( lstat(fname, &thead->file_stats) != 0 )
    {
        fprintf(stderr, "Error: Could not stat %s\n", fname);
        exit(-1);
    }
    if (S_ISDIR(thead->file_stats.st_mode) != 0)
    {
        thead->ftype = JTARDIR;
    }
    else if (S_ISREG(thead->file_stats.st_mode) != 0)
    {
        thead->ftype = JTARNORMAL;
    }
    else if (S_ISLINK(thead->file_stats.st_mode) != 0)
    {
        thead->ftype = JTARSYMLINK;
    }
    else
    {
        fprintf(stderr, "Error: Could not determine file type of %s\n", fname);
        exit(-1);
    }
    thead->checksum = set_checksum(thead);
    if (thead->file_stats.st_nlink > 1)
    {
        thead->hard_links = (char**) malloc((thead->file_stats.st_nlink-1)*sizeof(char*));
        for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
        {
            thead->hard_links[i] = (char*) malloc(100);
            thead->hard_links[i][0] = 0;
        }
    }
    else
    {
        thead->hard_links = (char**) malloc(sizeof(char*));
        thead->hard_links[0] = (char*) malloc(1);
        thead->hard_links[0][0] = 0;
    }
    thead->linknum = 0;
    finfo->header_for_tar = thead;
    return finfo;
}

TarHeader* parse_header(char *head)
{
    TarHeader *thead = (TarHeader*) malloc(sizeof(TarHeader));
    memcpy(thead->tar_name, &head[0], 100);
    memcpy(&thead->ftype, &head[100], 1);
    memcpy(&thead->file_stats, &head[101], 144);
    memcpy(&thead->checksum, &head[245], 8);
    int64_t calc_sum = calc_checksum(thead);
    if (thead->checksum != calc_sum)
    {
        fprintf(stderr, "Error: Tarfile corrupted! Checksums don't match.\n");
        exit(-1);
    }
    if (thead->file_stats.st_nlink > 1)
    {
        thead->hard_links = (char**) malloc((thead->file_stats.st_nlink-1)*sizeof(char*));
        for (int i = 0; i < (int)(thead->file_stats.st_nlink-1); i++)
        {
            thead->hard_links[i] = (char*) malloc(100);
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
    if (thead->file_stats.st_nlink > 1)
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
    if (strcmp(h1->tar_name, h2->tar_name) != 0)
    {
        return false;
    }
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
