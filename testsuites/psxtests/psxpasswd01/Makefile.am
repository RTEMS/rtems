
rtems_tests_PROGRAMS = psxpasswd01
psxpasswd01_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxpasswd01.scn
dist_rtems_tests_DATA += psxpasswd01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxpasswd01_OBJECTS)
LINK_LIBS = $(psxpasswd01_LDLIBS)

psxpasswd01$(EXEEXT): $(psxpasswd01_OBJECTS) $(psxpasswd01_DEPENDENCIES)
	@rm -f psxpasswd01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
