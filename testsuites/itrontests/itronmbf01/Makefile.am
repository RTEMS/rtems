##
## $Id$
##

TEST = itronmbf01
PGM = ${ARCH}/$(TEST).exe

MANAGERS = all

C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

H_FILES = system.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/itrontests.am

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../automake/local.am
