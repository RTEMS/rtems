##
## $Id$
##

MANAGERS = io

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c first.c fptask.c task1.c system.h fptest.h inttest.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = sp19.scn
docdir = $(testsdir)
dist_doc_DATA = sp19.doc

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/sp19.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
