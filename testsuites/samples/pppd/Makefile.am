##
##  $Id$
##

PGM = ${ARCH}/pppd.exe

MANAGERS = io event semaphore

noinst_LIBRARIES = lib.a
lib_a_SOURCES = init.c pppdapp.c netconfig.h system.h

OBJS = $(lib_a_OBJECTS)

testsdir = $(libdir)/tests

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

$(PGM): ${ARCH}/$(dirstamp)

LD_LIBS = -lpppd

tests_SCRIPTS = $(PGM)

$(PGM): $(OBJS) $(LINK_FILES)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
