#
#  $Id$
#
# Templates/Makefile.leaf
# 	Template leaf node Makefile
#

# if you have CEXP set this variable to 'YES'
# and some "help" info will be compiled in.
HAVE_CEXP=YES

# C source names, if any, go here -- minus the .c
C_PIECES_YES=rpcio nfs sock_mbuf xdr_mbuf dirutils rpcio.modini nfs.modini cexphelp
C_PIECES_NO=rpcio nfs sock_mbuf xdr_mbuf

C_PIECES=$(C_PIECES_$(HAVE_CEXP))

C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

# C++ source names, if any, go here -- minus the .cc
CC_PIECES=
CC_FILES=$(CC_PIECES:%=%.cc)
CC_O_FILES=$(CC_PIECES:%=${ARCH}/%.o)

H_FILES=librtemsNfs.h rpcio.h
INST_HEADERS=librtemsNfs.h

# Assembly source names, if any, go here -- minus the .S
S_PIECES=
S_FILES=$(S_PIECES:%=%.S)
S_O_FILES=$(S_FILES:%.S=${ARCH}/%.o)

SRCS=$(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES) $(wildcard ../proto/$(ARCH)/*.o)

PGMS=${ARCH}/nfs.obj ${ARCH}/rpcio.obj ${ARCH}/dirutils.obj 

LIBNAME=librtemsNfs.a

LIB=$(ARCH)/$(LIBNAME)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(RTEMS_ROOT)/make/lib.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

DEFS_CEXP_YES=-DHAVE_CEXP

DEFINES  += $(DEFS_CEXP_$(HAVE_CEXP))

CPPFLAGS += -I. -I../proto

CFLAGS   += 

#
# CFLAGS_DEBUG_V are used when the `make debug' target is built.
# To link your application with the non-optimized RTEMS routines,
# uncomment the following line:
# CFLAGS_DEBUG_V += -qrtems_debug
#

LDFLAGS   +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

#CLEAN_ADDITIONS += xxx-your-debris-goes-here
CLOBBER_ADDITIONS +=

ifndef RTEMS_SITE_INSTALLDIR
RTEMS_SITE_INSTALLDIR = $(PROJECT_RELEASE)
endif

%nfs.obj: %nfs.o %nfs.modini.o
	$(LD) -r -o $@ $^ -L../proto/$(ARCH) -lnfsprot

%rpcio.obj: %rpcio.o %sock_mbuf.o %xdr_mbuf.o %rpcio.modini.o
	$(LD) -r -o $@ $^

%dirutils.obj: %dirutils.o
	$(LD) -r -o $@ $^

$(LIB): $(OBJS)
	$(make-library)

all:	${ARCH} $(SRCS) $(PGMS) $(LIB)

tar:
	echo not implemented

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL_CHANGE)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGMS} ${RTEMS_SITE_INSTALLDIR}/bin
	$(INSTALL_VARIANT) -m 555 ${LIB}  ${RTEMS_SITE_INSTALLDIR}/lib
	$(INSTALL_CHANGE) -m 444 ${INST_HEADERS}  ${RTEMS_SITE_INSTALLDIR}/include
