##
##  $Id$
##


LIBNAME = libblock
LIB = ${ARCH}/${LIBNAME}.a

C_FILES = bdbuf.c blkdev.c diskdevs.c ramdisk.c

C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/multilib.am
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

AM_CFLAGS += $(LIBC_DEFINES)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
