##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
TEST = ffitest

MANAGERS = io

C_FILES = init.c ffitest.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = system.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = scn
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg
include $(top_srcdir)/libffi.am

#
# (OPTIONAL) Add local stuff here using +=
#

LD_LIBS += -lffi

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all: ${ARCH} $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
