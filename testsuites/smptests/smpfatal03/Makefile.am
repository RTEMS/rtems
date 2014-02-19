rtems_tests_PROGRAMS = smpfatal03
smpfatal03_SOURCES = init.c

dist_rtems_tests_DATA = smpfatal03.scn smpfatal03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpfatal03_OBJECTS)
LINK_LIBS = $(smpfatal03_LDLIBS)

smpfatal03$(EXEEXT): $(smpfatal03_OBJECTS) $(smpfatal03_DEPENDENCIES)
	@rm -f smpfatal03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
