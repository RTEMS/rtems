##
##  $Id$
##

LIB = ${ARCH}/libblock.a

C_FILES = bdbuf.c blkdev.c diskdevs.c ramdisk.c

C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

AM_CFLAGS += $(LIBC_DEFINES)

$(LIB): ${OBJS}
	$(make-library)

all-local: ${ARCH} $(LIB)

include $(top_srcdir)/../automake/local.am
