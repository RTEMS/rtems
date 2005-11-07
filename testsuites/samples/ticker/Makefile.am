##
##  $Id$
##

SAMPLE = ticker
PGM = ${ARCH}/$(SAMPLE).exe

MANAGERS = io

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c tasks.c system.h

DOCTYPES = doc scn
DOCS = $(DOCTYPES:%=$(SAMPLE).%)

SRCS = $(DOCS)
OBJS = $(lib_a_OBJECTS)

PRINT_SRCS = $(DOCS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sample.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(TMPINSTALL_FILES)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
