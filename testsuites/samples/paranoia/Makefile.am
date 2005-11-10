##
##  $Id$
##

TEST = paranoia
PGM = ${ARCH}/$(TEST).exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c paranoia.c system.h

DOCTYPES = doc
DOCS = $(DOCTYPES:%=$(TEST).%)

OBJS = $(lib_a_OBJECTS)


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

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
