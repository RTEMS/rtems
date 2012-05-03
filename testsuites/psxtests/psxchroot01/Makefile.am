
rtems_tests_PROGRAMS = psxchroot01
psxchroot01_SOURCES = main.c test.c ../include/pmacros.h

dist_rtems_tests_DATA = psxchroot01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxchroot01_OBJECTS)
LINK_LIBS = $(psxchroot01_LDLIBS)

psxchroot01$(EXEEXT): $(psxchroot01_OBJECTS) $(psxchroot01_DEPENDENCIES)
	@rm -f psxchroot01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
