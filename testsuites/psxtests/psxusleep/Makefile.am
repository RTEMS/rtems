
rtems_tests_PROGRAMS = psxusleep
psxusleep_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxusleep.scn
dist_rtems_tests_DATA += psxusleep.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxusleep_OBJECTS)
LINK_LIBS = $(psxusleep_LDLIBS)

psxusleep$(EXEEXT): $(psxusleep_OBJECTS) $(psxusleep_DEPENDENCIES)
	@rm -f psxusleep$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
