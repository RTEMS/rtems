#
#  Makefile.lib,v 1.5 2000/06/12 15:00:14 joel Exp
#
# Templates/Makefile.lib
#       Template library Makefile
#

LIBNAME=libif_em.a        # xxx- your library names goes here
#PGMS=${ARCH}/if_em.obj
LIB=${ARCH}/${LIBNAME}

# C and C++ source names, if any, go here -- minus the .c or .cc
C_PIECES=if_em_hw if_em if_em_rtems
#C_PIECES=if_em_hw if_em if_em_rtems if_em.modini
MODOBJS=$(ARCH)/if_em.o $(ARCH)/if_em_hw.o $(ARCH)/if_em.modini.o
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

CC_PIECES=
CC_FILES=$(CC_PIECES:%=%.cc)
CC_O_FILES=$(CC_PIECES:%=${ARCH}/%.o)

H_FILES=

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

DEFINES  += -DHAVE_LIBBSPEXT
CPPFLAGS += -I. -Ilibchip -Iporting
CFLAGS   += -Wno-unused-variable -msoft-float

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += 
CLOBBER_ADDITIONS +=

all:	${ARCH} $(SRCS) $(LIB) ${PGMS}

# doesn't work if we define this just after OBJS= :-(

# must be after inclusion of RTEMS_CUSTOM

$(LIB):  OBJS=$(filter-out %.modini.o,$(OBJS))

$(LIB): ${OBJS}
	$(make-library)

#How to make a relocatable object
$(filter %.obj, $(PGMS)): $(MODOBJS)
	$(make-obj)

ifndef RTEMS_SITE_INSTALLDIR
RTEMS_SITE_INSTALLDIR = $(PROJECT_RELEASE)
endif

${RTEMS_SITE_INSTALLDIR}/include \
${RTEMS_SITE_INSTALLDIR}/lib \
${RTEMS_SITE_INSTALLDIR}/bin \
${RTEMS_SITE_INSTALLDIR}/$(RTEMS_BSP)/include \
${RTEMS_SITE_INSTALLDIR}/$(RTEMS_BSP)/lib \
${RTEMS_SITE_INSTALLDIR}/$(RTEMS_BSP)/bin :
	test -d $@ || mkdir -p $@

# Install the library, appending _g or _p as appropriate.
# for include files, just use $(INSTALL_CHANGE)
#
# NOTES:
#  - BSP specific libraries, headers etc. should be installed to 
#          $RTEMS_SITE_INSTALLDIR)/$(RTEMS_BSP)/lib
#

install:  all $(RTEMS_SITE_INSTALLDIR)/lib
	$(INSTALL_VARIANT) -m 644 ${LIB} ${RTEMS_SITE_INSTALLDIR}/lib
