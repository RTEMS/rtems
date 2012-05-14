
rtems_tests_PROGRAMS = psximfs01
psximfs01_SOURCES = init.c

dist_rtems_tests_DATA = psximfs01.scn
dist_rtems_tests_DATA += psximfs01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psximfs01_OBJECTS)
LINK_LIBS = $(psximfs01_LDLIBS)

psximfs01$(EXEEXT): $(psximfs01_OBJECTS) $(psximfs01_DEPENDENCIES)
	@rm -f psximfs01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
