rtems_tests_PROGRAMS = smpschededf01
smpschededf01_SOURCES = init.c

dist_rtems_tests_DATA = smpschededf01.scn smpschededf01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpschededf01_OBJECTS)
LINK_LIBS = $(smpschededf01_LDLIBS)

smpschededf01$(EXEEXT): $(smpschededf01_OBJECTS) $(smpschededf01_DEPENDENCIES)
	@rm -f smpschededf01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
