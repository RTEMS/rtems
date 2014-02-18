rtems_tests_PROGRAMS = smpfatal02
smpfatal02_SOURCES = init.c

dist_rtems_tests_DATA = smpfatal02.scn smpfatal02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpfatal02_OBJECTS)
LINK_LIBS = $(smpfatal02_LDLIBS)

smpfatal02$(EXEEXT): $(smpfatal02_OBJECTS) $(smpfatal02_DEPENDENCIES)
	@rm -f smpfatal02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
