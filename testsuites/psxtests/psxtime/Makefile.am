##
## $Id$
##

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = main.c test.c

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = psxtime.scn

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/psxtime.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
