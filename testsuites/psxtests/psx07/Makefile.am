##
## $Id$
##

TEST = psx07

MANAGERS = all

C_FILES = init.c task.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

H_FILES = system.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = scn
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/psxtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -I$(top_srcdir)/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
