##
##  $Id$
##

NODE = 2
SAMPLE = base_mp-node$(NODE)
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io mp

C_FILES = ../init.c ../apptask.c
C_O_FILES = $(C_FILES:../%.c=${ARCH}/%.$(OBJEXT))

$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): ../%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

H_FILES = system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=base_mp.%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

AM_CPPFLAGS += -DNODE_NUMBER=$(NODE) -I.

if HAS_MP

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
