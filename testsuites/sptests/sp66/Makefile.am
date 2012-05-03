
rtems_tests_PROGRAMS = sp66
sp66_SOURCES = ../sp65/init.c

dist_rtems_tests_DATA = sp66.scn
dist_rtems_tests_DATA += sp66.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DINHERIT_CEILING

LINK_OBJS = $(sp66_OBJECTS)
LINK_LIBS = $(sp66_LDLIBS)

sp66$(EXEEXT): $(sp66_OBJECTS) $(sp66_DEPENDENCIES)
	@rm -f sp66$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
