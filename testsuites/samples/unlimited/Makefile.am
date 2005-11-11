##
##  $Id$
##

PGM = ${ARCH}/unlimited.exe

MANAGERS = io event

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c test1.c test2.c test3.c system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = unlimited.scn
docdir = $(testsdir)
dist_doc_DATA = unlimited.doc

OBJS = $(lib_a_OBJECTS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
