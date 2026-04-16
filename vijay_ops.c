// vijay_ops.c
// Vijay: FUSE scaffold + getattr + readdir + read

#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

struct mini_unionfs_state {
    char lower[512];
    char upper[512];
};

#define UNIONFS_DATA ((struct mini_unionfs_state *) fuse_get_context()->private_data)

int mini_getattr(const char *path, struct stat *stbuf)
{
    char fpath[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(fpath,"%s%s",state->upper,path);
    if(lstat(fpath,stbuf)==0)
        return 0;

    sprintf(fpath,"%s%s",state->lower,path);
    if(lstat(fpath,stbuf)==0)
        return 0;

    return -ENOENT;
}

int mini_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi,
                        enum fuse_readdir_flags flags)
{
    DIR *dp;
    struct dirent *de;
    char fpath[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    filler(buf,".",NULL,0,0);
    filler(buf,"..",NULL,0,0);

    sprintf(fpath,"%s%s",state->upper,path);
    dp = opendir(fpath);

    if(dp){
        while((de=readdir(dp))!=NULL){
            filler(buf,de->d_name,NULL,0,0);
        }
        closedir(dp);
    }

    sprintf(fpath,"%s%s",state->lower,path);
    dp = opendir(fpath);

    if(dp){
        while((de=readdir(dp))!=NULL){
            filler(buf,de->d_name,NULL,0,0);
        }
        closedir(dp);
    }

    return 0;
}

int mini_read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(fpath,"%s%s",state->upper,path);
    fd=open(fpath,O_RDONLY);

    if(fd==-1){
        sprintf(fpath,"%s%s",state->lower,path);
        fd=open(fpath,O_RDONLY);
        if(fd==-1)
            return -errno;
    }

    res=pread(fd,buf,size,offset);
    if(res==-1)
        res=-errno;

    close(fd);
    return res;
}
