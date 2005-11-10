##
## $Id$
##

TEST = tm26

MANAGERS = io semaphore

noinst_LIBRARIES = lib.a
lib_a_SOURCES = task1.c system.h fptest.h ../include/timesys.h ../include/timesys.h

DOCS = $(TEST).doc

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
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
