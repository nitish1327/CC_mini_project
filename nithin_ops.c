
// nithin_ops.c
// Nithin: Copy-on-Write open + unlink with whiteout

#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

struct mini_unionfs_state {
    char lower[512];
    char upper[512];
};

#define UNIONFS_DATA ((struct mini_unionfs_state *) fuse_get_context()->private_data)

int copy_to_upper(const char *path)
{
    char lower[1024],upper[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(lower,"%s%s",state->lower,path);
    sprintf(upper,"%s%s",state->upper,path);

    FILE *src=fopen(lower,"r");
    if(!src) return -errno;

    FILE *dst=fopen(upper,"w");
    if(!dst){
        fclose(src);
        return -errno;
    }

    char buf[4096];
    size_t n;

    while((n=fread(buf,1,sizeof(buf),src))>0)
        fwrite(buf,1,n,dst);

    fclose(src);
    fclose(dst);

    return 0;
}

int mini_open(const char *path, struct fuse_file_info *fi)
{
    char upper[1024],lower[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(upper,"%s%s",state->upper,path);
    sprintf(lower,"%s%s",state->lower,path);

    if((fi->flags & O_WRONLY) || (fi->flags & O_RDWR))
    {
        if(access(upper,F_OK)!=0 && access(lower,F_OK)==0)
        {
            copy_to_upper(path);
        }
    }

    return 0;
}

int mini_unlink(const char *path)
{
    char upper[1024],whiteout[1024];
    struct mini_unionfs_state *state = UNIONFS_DATA;

    sprintf(upper,"%s%s",state->upper,path);

    if(access(upper,F_OK)==0)
        return unlink(upper);

    sprintf(whiteout,"%s/.wh.%s",state->upper,path+1);

    int fd=open(whiteout,O_CREAT,0644);
    if(fd<0)
        return -errno;

    close(fd);
    return 0;
}
