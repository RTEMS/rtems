
rtems_tests_PROGRAMS = psxenosys
psxenosys_SOURCES = init.c system.h ../include/pmacros.h

dist_rtems_tests_DATA = psxenosys.scn
dist_rtems_tests_DATA += psxenosys.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxenosys_OBJECTS)
LINK_LIBS = $(psxenosys_LDLIBS)

psxenosys$(EXEEXT): $(psxenosys_OBJECTS) $(psxenosys_DEPENDENCIES)
	@rm -f psxenosys$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
