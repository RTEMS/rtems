
rtems_tests_PROGRAMS = psxcancel
psxcancel_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxcancel.scn
dist_rtems_tests_DATA += psxcancel.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxcancel_OBJECTS)
LINK_LIBS = $(psxcancel_LDLIBS)

psxcancel$(EXEEXT): $(psxcancel_OBJECTS) $(psxcancel_DEPENDENCIES)
	@rm -f psxcancel$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
