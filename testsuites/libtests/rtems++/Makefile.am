##
## $Id$
##


TEST = rtems++

MANAGERS = event io msg sem signal timer rate_monotonic

CC_FILES = Init.cc Task1.cc Task2.cc Task3.cc
OBJS = $(CC_FILES:%.cc=${ARCH}/%.$(OBJEXT))

H_FILES = System.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = scn doc
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/leaf.am
include $(top_srcdir)/libtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

LD_LIBS = -lrtems++$(LIB_VARIANT)

if HAS_CXX

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)

all-local: $(ARCH) $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(CC_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
