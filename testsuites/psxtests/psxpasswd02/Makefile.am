
rtems_tests_PROGRAMS = psxpasswd02
psxpasswd02_SOURCES = init.c ../include/pmacros.h

dist_rtems_tests_DATA = psxpasswd02.scn
dist_rtems_tests_DATA += psxpasswd02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxpasswd02_OBJECTS)
LINK_LIBS = $(psxpasswd02_LDLIBS)

psxpasswd02$(EXEEXT): $(psxpasswd02_OBJECTS) $(psxpasswd02_DEPENDENCIES)
	@rm -f psxpasswd02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
