##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

TEST = termios

MANAGERS = io

C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES =
noinst_HEADERS = $(H_FILES)

DOCTYPES =
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS = $(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/leaf.am
include $(top_srcdir)/libtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(ARCH) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
