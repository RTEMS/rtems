#
#  $Id$
#
# Templates/Makefile.lib
#       Template library Makefile
#


LIBNAME=libnfsprot.a        # xxx- your library names goes here
LIB=${ARCH}/${LIBNAME}

# C and C++ source names, if any, go here -- minus the .c or .cc
C_PIECES=nfs_prot_xdr mount_prot_xdr
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

CC_PIECES=
CC_FILES=$(CC_PIECES:%=%.cc)
CC_O_FILES=$(CC_PIECES:%=${ARCH}/%.o)

H_FILES=

XFILES = $(wildcard *.x)

# Assembly source names, if any, go here -- minus the .S
S_PIECES=
S_FILES=$(S_PIECES:%=%.S)
S_O_FILES=$(S_FILES:%.S=${ARCH}/%.o)

SRCS=$(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc

include $(RTEMS_CUSTOM)
include $(RTEMS_ROOT)/make/lib.cfg

#
# Add local stuff here using +=
#

DEFINES  +=
CPPFLAGS +=
# inline declarations require -O
# rpcgen produces unused variables
CFLAGS   += -O2 -Winline -Wno-unused-variable

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS +=
CLOBBER_ADDITIONS += $(XFILES:%.x=%*.c) $(XFILES:%.x=%*.h)

all:	${ARCH} $(SRCS) $(LIB)

$(LIB): ${OBJS}
	$(make-library)

%_xdr.c: %.x %.h
	rpcgen -c -C $< > $@

%.h: %.x
	rpcgen -h -C $< > $@

.PRECIOUS: $(XFILES:%.x=%.h) $(XFILES:%.x=%_xdr.c)

tar: $(XFILES:%.x=%.h) $(XFILES:%.x=%_xdr.c)

distclean: clean

# DONT install this library.
install:  all
