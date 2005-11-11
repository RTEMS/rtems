##
## $Id$
##

MANAGERS = io signal timer

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c asr.c task1.c task2.c system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = sp14.scn
docdir = $(testsdir)
dist_doc_DATA = sp14.doc

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/sp14.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
