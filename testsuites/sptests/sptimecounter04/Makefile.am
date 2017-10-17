rtems_tests_PROGRAMS = sptimecounter04
sptimecounter04_SOURCES = init.c

dist_rtems_tests_DATA = sptimecounter04.scn sptimecounter04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimecounter04_OBJECTS)
LINK_LIBS = $(sptimecounter04_LDLIBS)

sptimecounter04$(EXEEXT): $(sptimecounter04_OBJECTS) $(sptimecounter04_DEPENDENCIES)
	@rm -f sptimecounter04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
