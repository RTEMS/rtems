
rtems_tests_PROGRAMS = psxcancel01
psxcancel01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxcancel01.scn
dist_rtems_tests_DATA += psxcancel01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcancel01_OBJECTS)
LINK_LIBS = $(psxcancel01_LDLIBS)

psxcancel01$(EXEEXT): $(psxcancel01_OBJECTS) $(psxcancel01_DEPENDENCIES)
	@rm -f psxcancel01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
