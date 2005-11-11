##
## $Id$
##

MANAGERS = all

noinst_LIBRARIES = lib.a
lib_a_SOURCES = psxtimer.c system.h ../include/pmacros.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = psxtimer.scn

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/psxtimer.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/include

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
