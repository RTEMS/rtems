##
##  $Id$
##

SAMPLE = minimum
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = all

C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(TMPINSTALL_FILES)
# 	This test should not be executed

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../automake/local.am
