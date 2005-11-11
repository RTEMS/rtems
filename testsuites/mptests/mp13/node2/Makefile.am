##
## $Id$
##

PGM = $(ARCH)/mp13-node2.exe

MANAGERS = io mp message semaphore

noinst_LIBRARIES = lib.a
lib_a_SOURCES = ../init.c ../task1.c ../task2.c ../system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = mp13-node2.scn
docdir = $(testsdir)
dist_doc_DATA = mp13-node2.doc

OBJS = $(lib_a_OBJECTS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -DNODE_NUMBER=2 -I$(srcdir)/..

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
