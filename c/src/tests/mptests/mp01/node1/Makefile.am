##
## $Id$
##


VPATH = @srcdir@:@srcdir@/..

NODE = 1
TEST = mp01
PGM = ${ARCH}/$(TEST)-node$(NODE).exe

MANAGERS = io mp

C_FILES = init.c task1.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

H_FILES = system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/mptests.am

AM_CPPFLAGS += -DNODE_NUMBER=$(NODE) -I$(srcdir)/..

if HAS_MP

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(ARCH) $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(DOCS)

include $(top_srcdir)/../automake/local.am
