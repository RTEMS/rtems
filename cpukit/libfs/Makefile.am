##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

AM_CPPFLAGS += -I$(top_builddir)
AM_CPPFLAGS += $(LIBC_DEFINES)

EXTRA_DIST = README

include_HEADERS =

# imfs

libimfs_EMBEDDED_C_FILES = src/imfs/imfs_chown.c src/imfs/imfs_config.c \
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

libimfs_UNUSED_C_FILES = src/imfs/imfs_load_tar.c

libimfs_UNIX_C_FILES = src/imfs/imfs_unixstub.c

if UNIX
libimfs_C_FILES = $(libimfs_UNIX_C_FILES)
else
libimfs_C_FILES = $(libimfs_EMBEDDED_C_FILES)
endif
libimfs_OBJS = $(libimfs_C_FILES:src/imfs/%.c=src/imfs/$(ARCH)/%.$(OBJEXT))

include_HEADERS += src/imfs/imfs.h

IMFSLIB = src/imfs/$(ARCH)/libimfs.a

src/imfs/$(ARCH)/$(dirstamp):
	@$(mkdir_p) src/imfs/$(ARCH)
	@: > src/imfs/$(ARCH)/$(dirstamp)
src/imfs/$(ARCH)/%.$(OBJEXT): src/imfs/%.c src/imfs/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
src/imfs/$(ARCH)/libimfs.a: ${libimfs_OBJS}
	$(make-library)

EXTRA_DIST += $(libimfs_EMBEDDED_C_FILES) $(libimfs_UNIX_C_FILES)

# dosfs

noinst_HEADERS = src/dosfs/fat.h

libdosfs_FATFS_C_FILES = src/dosfs/fat.c src/dosfs/fat_fat_operations.c \
    src/dosfs/fat_file.c

libdosfs_DOSFS_C_FILES = src/dosfs/msdos_create.c src/dosfs/msdos_dir.c \
    src/dosfs/msdos_eval.c src/dosfs/msdos_file.c src/dosfs/msdos_free.c \
    src/dosfs/msdos_fsunmount.c src/dosfs/msdos_handlers_dir.c \
    src/dosfs/msdos_handlers_file.c src/dosfs/msdos_init.c \
    src/dosfs/msdos_initsupp.c src/dosfs/msdos_misc.c \
    src/dosfs/msdos_mknod.c src/dosfs/msdos_node_type.c \
    src/dosfs/msdos_conv.c

if !UNIX
libdosfs_C_FILES = $(libdosfs_FATFS_C_FILES) $(libdosfs_DOSFS_C_FILES)
libdosfs_OBJS = \
    $(libdosfs_C_FILES:src/dosfs/%.c=src/dosfs/${ARCH}/%.$(OBJEXT))

include_HEADERS += src/dosfs/dosfs.h

DOSFSLIB = src/dosfs/$(ARCH)/libdosfs.a

src/dosfs/$(ARCH)/$(dirstamp):
	@$(mkdir_p) src/dosfs/$(ARCH)
	@: > src/dosfs/$(ARCH)/$(dirstamp)
src/dosfs/$(ARCH)/%.$(OBJEXT): src/dosfs/%.c src/dosfs/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
src/dosfs/$(ARCH)/libdosfs.a: ${libdosfs_OBJS}
	$(make-library)
endif

all-local: $(IMFSLIB) $(DOSFSLIB)

EXTRA_DIST += $(libdosfs_DOSFS_C_FILES) $(libdosfs_FATFS_C_FILES)

CLEANDIRS = src

# ---

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/imfs.h: src/imfs/imfs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/imfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/imfs.h

if !UNIX
$(PROJECT_INCLUDE)/dosfs.h: src/dosfs/dosfs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/dosfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/dosfs.h
endif

include $(top_srcdir)/automake/local.am
