##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

TEST = rtems++

MANAGERS = event io msg sem signal timer rate_monotonic

CC_FILES = Init.cc Task1.cc Task2.cc Task3.cc
CC_O_FILES = $(CC_FILES:%.cc=${ARCH}/%.o)

H_FILES = System.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = scn doc
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(CC_O_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg
include $(top_srcdir)/libtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

LD_LIBS += $(CPLUS_LD_LIBS)

if HAS_CXX
${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all: $(ARCH) $(TMPINSTALL_FILES)
else
all:
endif

EXTRA_DIST = $(CC_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
