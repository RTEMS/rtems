##
##  $Id$
##

SAMPLE = pppd
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io event semaphore

C_FILES = init.c pppdapp.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

H_FILES = netconfig.h system.h

SRCS = $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

if HAS_NETWORKING
all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)
endif

EXTRA_DIST = $(C_FILES) $(H_FILES)

include $(top_srcdir)/../automake/local.am
