##
##  $Id$
##


SAMPLE = cdtest
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = all

C_FILES = init.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

CC_FILES = main.cc
CC_O_FILES = $(CC_FILES:%.cc=${ARCH}/%.o)

H_FILES = system.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = scn
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

SRCS = $(DOCS) $(C_FILES) $(CC_FILES) $(H_FILES)
OBJS = $(C_O_FILES) $(CC_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

if HAS_CXX
${PGM}: $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)

all-local: ${ARCH} $(TMPINSTALL_FILES)
else
all-local:
endif

EXTRA_DIST = $(C_FILES) $(CC_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
