##
##  $Id$
##

NODE = 2
TEST = base_mp-node$(NODE)
PGM = ${ARCH}/$(TEST).exe

MANAGERS = io mp

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../init.c ../apptask.c ../system.h

DOCS = $(TEST).doc $(TEST).scn

OBJS = $(lib_a_OBJECTS)


include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

AM_CPPFLAGS += -DNODE_NUMBER=$(NODE) -I.

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
