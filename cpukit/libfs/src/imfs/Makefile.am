##
##  $Id$
##

LIB = ${ARCH}/libimfs.a

IMFS_C_FILES = imfs_chown.c imfs_config.c imfs_creat.c imfs_directory.c \
    imfs_eval.c imfs_free.c imfs_fsunmount.c imfs_gtkn.c imfs_init.c \
    imfs_initsupp.c imfs_link.c imfs_mknod.c imfs_mount.c imfs_fchmod.c \
    imfs_unlink.c imfs_unmount.c imfs_utime.c imfs_ntype.c imfs_stat.c \
    imfs_getchild.c memfile.c linearfile.c deviceio.c imfs_handlers_device.c \
    imfs_handlers_directory.c imfs_handlers_link.c imfs_handlers_memfile.c \
    imfs_debug.c imfs_rmnod.c imfs_symlink.c imfs_readlink.c imfs_fdatasync.c \
    imfs_fcntl.c ioman.c miniimfs_init.c 

# imfs_load_tar.c

UNIX_C_FILES = imfs_unixstub.c

EMBEDDED_C_FILES = $(IMFS_C_FILES)

if UNIX
C_FILES = $(UNIX_C_FILES)
else
C_FILES = $(EMBEDDED_C_FILES)
endif
OBJS = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

noinst_HEADERS = config.h

include_HEADERS = imfs.h

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

AM_CPPFLAGS += $(LIBC_DEFINES)

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# Add local stuff here using +=
#

all-local: ${ARCH} $(LIB)

$(LIB): ${OBJS}
	$(make-library)

EXTRA_DIST = $(EMBEDDED_C_FILES) $(UNIX_C_FILES)

include $(top_srcdir)/automake/local.am
