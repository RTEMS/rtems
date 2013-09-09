rtems_tests_PROGRAMS = exit01
exit01_SOURCES = init.c

dist_rtems_tests_DATA = exit01.scn exit01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(exit01_OBJECTS)
LINK_LIBS = $(exit01_LDLIBS)

exit01$(EXEEXT): $(exit01_OBJECTS) $(exit01_DEPENDENCIES)
	@rm -f exit01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
