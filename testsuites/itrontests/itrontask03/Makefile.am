##
## $Id$
##

PGM = ${ARCH}/itrontask03.exe

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c preempt.c task1.c task2.c task3.c system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = itrontask03.scn
docdir = $(testsdir)
dist_doc_DATA = itrontask03.doc

OBJS = $(lib_a_OBJECTS)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
