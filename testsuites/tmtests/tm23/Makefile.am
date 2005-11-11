##
## $Id$
##

MANAGERS = io event timer

noinst_LIBRARIES = lib.a
lib_a_SOURCES = task1.c system.h ../include/timesys.h

testsdir = $(libdir)/tests
docdir = $(testsdir)
dist_doc_DATA = tm23.doc

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/tm23.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
