#include "tar_head.h"

bool dir_eq(char *dir1, char *dir2)
{
    if (strcmp(dir1, dir2) == 0)
    {
        return true;
    }
    struct stat *buf1, *buf2;
    if ( lstat(dir1, buf1) != 0 )
    {
        fprintf(stderr, "Error: Could not stat directory %s\n", dir1);
        exit(-1);
    }
    if ( lstat(dir2, buf2) != 0 )
    {
        fprintf(stderr, "Error: Could not stat directory %s\n", dir2);
        exit(-1);
    }
    return (buf1->st_dev == buf2->st_dev) && (buf1->st_ino == buf2->st_ino);
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
    sum += (int64_t) thead->mode;
    sum += thead->num_bytes;
    sum += thead->mod_time;
    return sum;
}

FileInfo* create_header(char *fname)
{
    FileInfo *finfo = (FileInfo*) malloc(sizeof(FileInfo));
    TarHeader *thead = (TarHeader*) malloc(sizeof(TarHeader));
    finfo->real_name = strdup(fname);
    thead->tar_name = remove_relative_specifiers_from_path(fname);
    struct stat *fstat;
    if ( lstat(fname, fstat) != 0 )
    {
        fprintf(stderr, "Error: Could not stat %s\n", fname);
        exit(-1);
    }
    finfo->file_stats = fstat;
    if (S_ISDIR(fstat->st_mode) != 0)
    {
        thead->ftype = JTARDIR;
    }
    else if (S_ISREG(fstat->st_mode) != 0)
    {
        thead->ftype = JTARNORMAL;
    }
    else if (S_ISLINK(fstat->st_mode) != 0)
    {
        thead->ftype = JTARSYMLINK;
    }
    else
    {
        fprintf(stderr, "Error: Could not determine file type of %s\n", fname);
        exit(-1);
    }
    thead->mode = fstat->st_mode;
    thead->num_bytes = (uint64_t) fstat->st_size;
    thead->mod_time = (uint64_t) fstat->st_mtime;
    thead->checksum = set_checksum(thead);
    finfo->header_for_tar = thead;
    return finfo;
}

TarHeader* parse_header(char *head)
{
    TarHeader *thead = (TarHeader*) malloc(sizeof(TarHeader));
    memcpy(thead->tar_name, &head[0], 100);
    memcpy(thead->ftype, &head[100], 1);
    memcpy(thead->mode, &head[101], 4);
    memcpy(thead->num_bytes, &head[105], 8);
    memcpy(thead->mod_time, &head[113], 8);
    memcpy(thead->checksum, &head[121], 8);
    int64_t calc_sum = calc_checksum(thead);
    if (thead->checksum != calc_sum)
    {
        fprintf(stderr, "Error: Tarfile corrupted! Checksums don't match.\n");
        exit(-1);
    }
    return thead;
}

void free_fileinfo(FileInfo *finfo)
{
    free(finfo->header_for_tar);
    free(finfo->file_stats);
    free(finfo);
}

bool header_eq(TarHeader *h1, TarHeader *h2)
{
    if (strcmp(h1->tar_name, h2->tar_name) != 0)
    {
        return false;
    }
    return (h1->ftype == h2->ftype) && (h1->mode == h2->mode) && 
           (h1->num_bytes == h2->num_bytes) && (h1->mod_time == h2->mod_time);
}
