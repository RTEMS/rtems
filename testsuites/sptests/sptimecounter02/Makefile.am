rtems_tests_PROGRAMS = sptimecounter02
sptimecounter02_SOURCES = init.c
sptimecounter02_SOURCES += ../../support/src/spin.c

dist_rtems_tests_DATA = sptimecounter02.scn sptimecounter02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimecounter02_OBJECTS)
LINK_LIBS = $(sptimecounter02_LDLIBS)

sptimecounter02$(EXEEXT): $(sptimecounter02_OBJECTS) $(sptimecounter02_DEPENDENCIES)
	@rm -f sptimecounter02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
