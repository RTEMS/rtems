##
##  $Id$
##

SAMPLE = paranoia
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = all

C_FILES = init.c paranoia.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

H_FILES = system.h
noinst_HEADERS = $(H_FILES)

DOCTYPES = doc
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#
#  -DCYGNUS   turns on more verbose msgs
#

AM_CPPFLAGS += -DNOSIGNAL -DCYGNUS -DNOMAIN

LD_LIBS = -lm

all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../automake/local.am
