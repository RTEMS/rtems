##
## $Id$
##

MANAGERS = io rate_monotonic

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c task1.c task2.c task3.c tswitch.c system.h

testsdir = $(libdir)/tests
scndir = $(testsdir)
dist_scn_DATA = cpuuse.scn

OBJS = $(lib_a_OBJECTS)

PGM = ${ARCH}/cpuuse.exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
