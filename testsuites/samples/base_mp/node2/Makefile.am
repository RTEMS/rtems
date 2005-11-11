##
##  $Id$
##

PGM = ${ARCH}/base_mp-node2.exe

MANAGERS = io mp

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../init.c ../apptask.c ../system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = base_mp-node2.scn
docdir = $(testsdir)
dist_doc_DATA = base_mp-node2.doc

OBJS = $(lib_a_OBJECTS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -DNODE_NUMBER=2 -I.

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
