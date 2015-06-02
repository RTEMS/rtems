rtems_tests_PROGRAMS = sptimecounter03
sptimecounter03_SOURCES = init.c

dist_rtems_tests_DATA = sptimecounter03.scn sptimecounter03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimecounter03_OBJECTS)
LINK_LIBS = $(sptimecounter03_LDLIBS)

sptimecounter03$(EXEEXT): $(sptimecounter03_OBJECTS) $(sptimecounter03_DEPENDENCIES)
	@rm -f sptimecounter03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
