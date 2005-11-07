##
## $Id$
##

TEST = tmoverhd

MANAGERS = io

noinst_LIBRARIES = lib.a
lib_a_SOURCES = testtask.c empty.c

lib_a_SOURCES += system.h dumrtems.h

DOCTYPES = doc
DOCS = $(DOCTYPES:%=$(TEST).%)

OBJS = $(lib_a_OBJECTS)


PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/tmtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
