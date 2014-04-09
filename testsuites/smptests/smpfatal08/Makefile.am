rtems_tests_PROGRAMS = smpfatal08
smpfatal08_SOURCES = init.c

dist_rtems_tests_DATA = smpfatal08.scn smpfatal08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpfatal08_OBJECTS)
LINK_LIBS = $(smpfatal08_LDLIBS)

smpfatal08$(EXEEXT): $(smpfatal08_OBJECTS) $(smpfatal08_DEPENDENCIES)
	@rm -f smpfatal08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
