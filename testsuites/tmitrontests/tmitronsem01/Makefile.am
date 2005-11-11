##
## $Id$
##

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c system.h

testsdir = $(libdir)/tests
docdir = $(testsdir)
dist_doc_DATA = tmitronsem01.doc

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/tmitronsem01.exe

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
