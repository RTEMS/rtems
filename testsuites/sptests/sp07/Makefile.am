##
## $Id$
##

TEST = sp07

MANAGERS = io

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c buffered_io.c task1.c task2.c task3.c task4.c taskexit.c \
    tcreate.c tdelete.c trestart.c tstart.c system.h

DOCS = $(TEST).scn $(TEST).doc

OBJS = $(lib_a_OBJECTS)


PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am
include $(top_srcdir)/sptests.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = $(DOCS)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
