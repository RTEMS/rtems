##
## $Id$
##


TEST = tm19

MANAGERS = io signal

C_FILES = task1.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

H_FILES = system.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = doc
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/tmtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(ARCH) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../automake/local.am
