##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

INCLUDES = -I.
AM_CFLAGS += $(LIBC_DEFINES) 

FATFS_C_FILES = fat.c fat_fat_operations.c fat_file.c

DOSFS_C_FILES = msdos_create.c msdos_dir.c msdos_eval.c msdos_file.c \
                msdos_free.c msdos_fsunmount.c msdos_handlers_dir.c  \
                msdos_handlers_file.c msdos_init.c msdos_initsupp.c  \
                msdos_misc.c msdos_mknod.c msdos_node_type.c

if !UNIX
LIBNAME = libdosfs
LIB = ${ARCH}/${LIBNAME}.a

C_FILES = $(FATFS_C_FILES) $(DOSFS_C_FILES)

C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

include_HEADERS = fat.h fat_fat_operations.h \
                  fat_file.h msdos.h dosfs.h

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

include $(top_srcdir)/../../../automake/local.am
