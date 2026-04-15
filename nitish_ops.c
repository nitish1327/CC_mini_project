
// nitish_ops.c
// Nitish: state struct + resolve_path + write + create + mkdir + rmdir

#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct mini_unionfs_state {
    char lower[4096];
    char upper[4096];
};

#define UNIONFS_DATA ((struct mini_unionfs_state *) fuse_get_context()->private_data)

void resolve_path(char *resolved,const char *path)
{
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(resolved,"%s%s",state->upper,path);

    if(access(resolved,F_OK)==0)
        return;

    sprintf(resolved,"%s%s",state->lower,path);
}

int mini_write(const char *path,const char *buf,size_t size,
                      off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[1024];

    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(fpath,"%s%s",state->upper,path);

    fd=open(fpath,O_WRONLY);

    if(fd==-1)
        return -errno;

    res=pwrite(fd,buf,size,offset);

    if(res==-1)
        res=-errno;

    close(fd);
    return res;
}

int mini_create(const char *path,mode_t mode,
                       struct fuse_file_info *fi)
{
    char fpath[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(fpath,"%s%s",state->upper,path);

    int fd=open(fpath,O_CREAT|O_WRONLY,mode);

    if(fd==-1)
        return -errno;

    close(fd);
    return 0;
}

int mini_mkdir(const char *path, mode_t mode)
{
    char fpath[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(fpath,"%s%s",state->upper,path);

    if(mkdir(fpath,mode)==-1)
        return -errno;

    return 0;
}

int mini_rmdir(const char *path)
{
    char fpath[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(fpath,"%s%s",state->upper,path);

    if(rmdir(fpath)==-1)
        return -errno;

    return 0;
}
