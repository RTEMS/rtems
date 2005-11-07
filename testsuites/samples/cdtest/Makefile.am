##
##  $Id$
##

SAMPLE = cdtest
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c main.cc system.h

DOCTYPES = scn
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

# AM_CPPFLAGS += -DRTEMS_TEST_IO_STREAM

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
