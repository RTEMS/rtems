##
##  $Id$
##


include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

AM_CPPFLAGS += -I../.. $(LIBC_DEFINES) 

FATFS_C_FILES = fat.c fat_fat_operations.c fat_file.c

DOSFS_C_FILES = msdos_create.c msdos_dir.c msdos_eval.c msdos_file.c \
                msdos_free.c msdos_fsunmount.c msdos_handlers_dir.c  \
                msdos_handlers_file.c msdos_init.c msdos_initsupp.c  \
                msdos_misc.c msdos_mknod.c msdos_node_type.c

if !UNIX
LIB = ${ARCH}/libdosfs.a

C_FILES = $(FATFS_C_FILES) $(DOSFS_C_FILES)

C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

include_HEADERS = dosfs.h

H_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

OBJS = $(C_O_FILES)

#
# Add local stuff here using +=
#
all-local: ${ARCH} $(LIB)

$(LIB): ${OBJS}
	$(make-library)
endif

TMPINSTALL_FILES += $(H_FILES)

EXTRA_DIST = $(DOSFS_C_FILES) $(FATFS_C_FILES)

include $(top_srcdir)/../automake/local.am
