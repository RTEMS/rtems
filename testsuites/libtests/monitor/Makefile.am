##
## $Id$
##

MANAGERS = dpmem event io msg mp part region sem signal timer rate_monotonic \
    ext

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c system.h

testsdir = $(libdir)/tests

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/monitor.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
