rtems_tests_PROGRAMS = sptimecounter01
sptimecounter01_SOURCES = init.c

dist_rtems_tests_DATA = sptimecounter01.scn sptimecounter01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimecounter01_OBJECTS)
LINK_LIBS = $(sptimecounter01_LDLIBS)

sptimecounter01$(EXEEXT): $(sptimecounter01_OBJECTS) $(sptimecounter01_DEPENDENCIES)
	@rm -f sptimecounter01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
