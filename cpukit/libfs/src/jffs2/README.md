Updating
========

This directory contains a port of the JFFS2 file system from Linux.

To update to a newer Linux version use this command in a Git clone of Linux to
generate the relevant patches:

```shell
  git format-patch v6.1..v9.99 -- \
    include/uapi/linux/jffs2.h \
    fs/jffs2/LICENCE \
    fs/jffs2/acl.h \
    fs/jffs2/build.c \
    fs/jffs2/compr.c \
    fs/jffs2/compr.h \
    fs/jffs2/compr_rtime.c \
    fs/jffs2/compr_rubin.c \
    fs/jffs2/compr_zlib.c \
    fs/jffs2/debug.c \
    fs/jffs2/debug.h \
    fs/jffs2/erase.c \
    fs/jffs2/gc.c \
    fs/jffs2/jffs2_fs_i.h \
    fs/jffs2/jffs2_fs_sb.h \
    fs/jffs2/nodelist.c \
    fs/jffs2/nodelist.h \
    fs/jffs2/nodemgmt.c \
    fs/jffs2/read.c \
    fs/jffs2/readinode.c \
    fs/jffs2/scan.c \
    fs/jffs2/summary.h \
    fs/jffs2/wbuf.c \
    fs/jffs2/write.c \
    fs/jffs2/xattr.h
```

The patches need a transformation of file paths from Linux to RTEMS:

```shell
  sed -i 's%/include/uapi%/cpukit/libfs/src/jffs2/include%g' 00*
  sed -i 's%/fs/jffs2%/cpukit/libfs/src/jffs2/src%g' 00*
```
