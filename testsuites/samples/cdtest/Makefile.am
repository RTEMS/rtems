##
##  $Id$
##

SAMPLE = cdtest
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = all

C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

CC_FILES = main.cc
CC_O_FILES = $(CC_FILES:%.cc=${ARCH}/%.$(OBJEXT))

H_FILES = system.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = scn
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

SRCS = $(DOCS) $(C_FILES) $(CC_FILES) $(H_FILES)
OBJS = $(C_O_FILES) $(CC_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

# AM_CPPFLAGS += -DRTEMS_TEST_IO_STREAM

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(CC_FILES) $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
