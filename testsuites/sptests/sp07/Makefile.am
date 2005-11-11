##
## $Id$
##

MANAGERS = io

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c buffered_io.c task1.c task2.c task3.c task4.c taskexit.c \
    tcreate.c tdelete.c trestart.c tstart.c system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = sp07.scn
docdir = $(testsdir)
dist_doc_DATA = sp07.doc

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/sp07.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
