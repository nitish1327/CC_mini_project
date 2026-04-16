
// main.c
// Connects all team member functions

#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <stdlib.h>
#include <stdio.h>

struct mini_unionfs_state {
    char lower[512];
    char upper[512];
};

extern int mini_getattr(const char *, struct stat *);
extern int mini_readdir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *, enum fuse_readdir_flags);
extern int mini_read(const char *, char *, size_t, off_t, struct fuse_file_info *);

extern int mini_write(const char *,const char *,size_t,off_t, struct fuse_file_info *);
extern int mini_create(const char *,mode_t, struct fuse_file_info *);
extern int mini_mkdir(const char *, mode_t);
extern int mini_rmdir(const char *);

extern int mini_open(const char *, struct fuse_file_info *);
extern int mini_unlink(const char *);

static struct fuse_operations mini_oper = {
    .getattr = mini_getattr,
    .readdir = mini_readdir,
    .read    = mini_read,
    .write   = mini_write,
    .create  = mini_create,
    .mkdir   = mini_mkdir,
    .rmdir   = mini_rmdir,
    .open    = mini_open,
    .unlink  = mini_unlink,
};

int main(int argc, char *argv[])
{
    struct mini_unionfs_state *state;

    state = malloc(sizeof(struct mini_unionfs_state));

    realpath(argv[1], state->lower);
    realpath(argv[2], state->upper);

    argv[1] = argv[3];
    argv[2] = NULL;
    argc--;

    return fuse_main(argc, argv, &mini_oper, state);
}
