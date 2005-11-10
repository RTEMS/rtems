##
##  $Id$
##

TEST = iostream
PGM = ${ARCH}/$(TEST).exe

MANAGERS = io

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.cc system.h

DOCS = $(TEST).doc $(TEST).scn

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-cxx-exe)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
