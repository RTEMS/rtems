##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

# Pick up nfsclient
if LIBNETWORKING
NFSCLIENT_SUBDIR= src/nfsclient
endif
SUBDIRS = $(NFSCLIENT_SUBDIR)
DIST_SUBDIRS = $(SUBDIRS)

EXTRA_DIST = README

noinst_LIBRARIES = libimfs.a

libimfs_a_SOURCES =

if UNIX
libimfs_a_SOURCES += src/imfs/imfs_unixstub.c
else
libimfs_a_SOURCES += src/imfs/imfs_chown.c src/imfs/imfs_config.c \
    src/imfs/imfs_creat.c src/imfs/imfs_directory.c src/imfs/imfs_eval.c \
    src/imfs/imfs_free.c src/imfs/imfs_fsunmount.c src/imfs/imfs_gtkn.c \
    src/imfs/imfs_init.c src/imfs/imfs_initsupp.c src/imfs/imfs_link.c \
    src/imfs/imfs_mknod.c src/imfs/imfs_mount.c src/imfs/imfs_fchmod.c \
    src/imfs/imfs_unlink.c src/imfs/imfs_unmount.c src/imfs/imfs_utime.c \
    src/imfs/imfs_ntype.c src/imfs/imfs_stat.c src/imfs/imfs_getchild.c \
    src/imfs/memfile.c src/imfs/deviceio.c \
    src/imfs/imfs_handlers_device.c src/imfs/imfs_handlers_directory.c \
    src/imfs/imfs_handlers_link.c src/imfs/imfs_handlers_memfile.c \
    src/imfs/imfs_debug.c src/imfs/imfs_rmnod.c src/imfs/imfs_symlink.c \
    src/imfs/imfs_readlink.c src/imfs/imfs_fdatasync.c src/imfs/imfs_fcntl.c \
    src/imfs/ioman.c src/imfs/miniimfs_init.c src/imfs/imfs_load_tar.c \
    src/imfs/imfs.h \
    src/imfs/deviceerrno.c \
    src/devfs/devfs_init.c src/devfs/devfs_eval.c src/devfs/devfs_mknod.c \
    src/devfs/devfs_show.c src/devfs/devfs_node_type.c \
    src/devfs/devopen.c src/devfs/devread.c src/devfs/devwrite.c \
    src/devfs/devclose.c src/devfs/devioctl.c src/devfs/devstat.c \
    src/devfs/devfs.h
endif

# dosfs

if LIBDOSFS
noinst_LIBRARIES += libdosfs.a

# libdosfs_FATFS_C_FILES
libdosfs_a_SOURCES = src/dosfs/fat.c src/dosfs/fat.h \
    src/dosfs/fat_fat_operations.c src/dosfs/fat_fat_operations.h \
    src/dosfs/fat_file.c src/dosfs/fat_file.h

# libdosfs_DOSFS_C_FILES
libdosfs_a_SOURCES += src/dosfs/msdos_create.c src/dosfs/msdos_dir.c \
    src/dosfs/msdos_eval.c src/dosfs/msdos_file.c src/dosfs/msdos_free.c \
    src/dosfs/msdos_fsunmount.c src/dosfs/msdos_handlers_dir.c \
    src/dosfs/msdos_handlers_file.c src/dosfs/msdos_init.c \
    src/dosfs/msdos_initsupp.c src/dosfs/msdos_misc.c \
    src/dosfs/msdos_mknod.c src/dosfs/msdos_node_type.c \
    src/dosfs/msdos_conv.c src/dosfs/msdos.h src/dosfs/msdos_format.c \
    src/dosfs/dosfs.h
endif

# ---
include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
