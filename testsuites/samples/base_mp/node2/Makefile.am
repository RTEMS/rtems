##
##  $Id$
##

NODE = 2
SAMPLE = base_mp-node$(NODE)
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io mp

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../init.c ../apptask.c ../system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=base_mp.%)

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
