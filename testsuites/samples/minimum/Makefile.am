##
##  $Id$
##

PGM = ${ARCH}/minimum.exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = minimum.scn
docdir = $(testsdir)
dist_doc_DATA = minimum.doc

OBJS = $(lib_a_OBJECTS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

# 	This test should not be executed

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
