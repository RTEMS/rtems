##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

INCLUDES = -I.

LIBNAME = libdosfs
LIB = ${ARCH}/${LIBNAME}.a

FATFS_C_FILES = fat.c fat_fat_operations.c fat_file.c

DOSFS_C_FILES = msdos_create.c msdos_dir.c msdos_eval.c msdos_file.c \
                msdos_free.c msdos_fsunmount.c msdos_handlers_dir.c  \
                msdos_handlers_file.c msdos_init.c msdos_initsupp.c  \
                msdos_misc.c msdos_mknod.c msdos_node_type.c


UNIX_C_FILES = msdos_unixstub.c

EMBEDDED_C_FILES = $(FATFS_C_FILES) $(DOSFS_C_FILES)

COMMON_C_FILES =

if UNIX
C_FILES = $(COMMON_C_FILES) $(UNIX_C_FILES)
else
C_FILES = $(COMMON_C_FILES) $(EMBEDDED_C_FILES)
endif
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

include_HEADERS = fat.h fat_fat_operations.h \
                  fat_file.h msdos.h dosfs.h
SYS_H_FILES =
RTEMS_H_FILES = 
noinst_HEADERS =

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems \
    $(PROJECT_INCLUDE)/sys $(include_HEADERS:%=$(PROJECT_INCLUDE)/%) \
    $(RTEMS_H_FILES:%=$(PROJECT_INCLUDE)/rtems/%) \
    $(SYS_H_FILES:%=$(PROJECT_INCLUDE)/sys/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/sys/%.h: %.h
	$(INSTALL_DATA) $< $@

OBJS = $(C_O_FILES)

#
# Add local stuff here using +=
#

AM_CFLAGS += $(LIBC_DEFINES) 

all-local: ${ARCH} $(LIB)

$(LIB): ${OBJS}
	$(make-library)

DOC_FILES = 

EXTRA_DIST = $(DOC_FILES) $(COMMON_C_FILES) $(EMBEDDED_C_FILES) \
    $(UNIX_C_FILES) $(RTEMS_H_FILES) $(SYS_H_FILES)

include $(top_srcdir)/../../../automake/local.am
