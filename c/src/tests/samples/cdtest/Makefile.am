##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

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

PGM = ${ARCH}/$(SAMPLE).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

if HAS_CXX
LD_LIBS += $(CPLUS_LD_LIBS)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all: ${ARCH} $(TMPINSTALL_FILES)
else
all:
endif

EXTRA_DIST = $(C_FILES) $(CC_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
