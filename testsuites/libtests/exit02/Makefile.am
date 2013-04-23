rtems_tests_PROGRAMS = exit02
exit02_SOURCES = init.c

dist_rtems_tests_DATA = exit02.scn exit02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(exit02_OBJECTS)
LINK_LIBS = $(exit02_LDLIBS)

exit02$(EXEEXT): $(exit02_OBJECTS) $(exit02_DEPENDENCIES)
	@rm -f exit02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
