##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -I$(top_builddir)

EXTRA_DIST = README

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS =

noinst_LIBRARIES = libimfs.a
libimfs_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

UNUSED_FILES = src/imfs/imfs_load_tar.c

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
    src/imfs/memfile.c src/imfs/linearfile.c src/imfs/deviceio.c \
    src/imfs/imfs_handlers_device.c src/imfs/imfs_handlers_directory.c \
    src/imfs/imfs_handlers_link.c src/imfs/imfs_handlers_memfile.c \
    src/imfs/imfs_debug.c src/imfs/imfs_rmnod.c src/imfs/imfs_symlink.c \
    src/imfs/imfs_readlink.c src/imfs/imfs_fdatasync.c src/imfs/imfs_fcntl.c \
    src/imfs/ioman.c src/imfs/miniimfs_init.c
endif

include_rtems_HEADERS += src/imfs/imfs.h

# dosfs

if !UNIX
noinst_LIBRARIES += libdosfs.a
libdosfs_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

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
    src/dosfs/msdos_conv.c src/dosfs/msdos.h src/dosfs/msdos_format.c

include_rtems_HEADERS += src/dosfs/dosfs.h
endif

EXTRA_DIST += $(UNUSED_FILES)

all-local: $(PREINSTALL_FILES)

# ---

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/imfs.h: src/imfs/imfs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/imfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/imfs.h

if !UNIX
$(PROJECT_INCLUDE)/rtems/dosfs.h: src/dosfs/dosfs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/dosfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/dosfs.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
