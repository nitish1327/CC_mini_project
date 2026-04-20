#!/bin/bash
gcc -g main.c nithin_ops.c nitish_ops.c vijay_ops.c -o mini_unionfs $(pkg-config fuse3 --cflags --libs) -D_FILE_OFFSET_BITS=64
