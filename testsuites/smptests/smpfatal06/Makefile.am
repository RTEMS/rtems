rtems_tests_PROGRAMS = smpfatal06
smpfatal06_SOURCES = init.c

dist_rtems_tests_DATA = smpfatal06.scn smpfatal06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpfatal06_OBJECTS)
LINK_LIBS = $(smpfatal06_LDLIBS)

smpfatal06$(EXEEXT): $(smpfatal06_OBJECTS) $(smpfatal06_DEPENDENCIES)
	@rm -f smpfatal06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
