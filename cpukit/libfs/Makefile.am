##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -I$(top_builddir)

EXTRA_DIST = README

include_HEADERS =

EXTRA_LIBRARIES = libimfs.a
CLEANFILES = libimfs.a
libimfs_a_CPPFLAGS = $(AM_CPPFLAGS) $(LIBC_DEFINES) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libimfs_g.a
CLEANFILES += libimfs_g.a
libimfs_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(LIBC_DEFINES) $(CFLAGS_DEBUG_V)

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

libimfs_g_a_SOURCES = $(libimfs_a_SOURCES)

include_HEADERS += src/imfs/imfs.h

IMFSLIB = libimfs$(LIB_VARIANT).a

# dosfs

if !UNIX
EXTRA_LIBRARIES += libdosfs.a
CLEANFILES += libdosfs.a
libdosfs_a_CPPFLAGS = $(AM_CPPFLAGS) $(LIBC_DEFINES) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libdosfs_g.a
CLEANFILES += libdosfs_g.a
libdosfs_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(LIBC_DEFINES) $(CFLAGS_DEBUG_V)

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
    src/dosfs/msdos_conv.c src/dosfs/msdos.h

libdosfs_g_a_SOURCES = $(libdosfs_a_SOURCES)

include_HEADERS += src/dosfs/dosfs.h

DOSFSLIB = libdosfs$(LIB_VARIANT).a
endif

EXTRA_DIST += $(UNUSED_FILES)

all-local: $(PREINSTALL_FILES) $(IMFSLIB) $(DOSFSLIB)

# ---

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/imfs.h: src/imfs/imfs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/imfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/imfs.h

if !UNIX
$(PROJECT_INCLUDE)/dosfs.h: src/dosfs/dosfs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/dosfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/dosfs.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
