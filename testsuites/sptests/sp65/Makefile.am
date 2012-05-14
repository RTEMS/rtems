
rtems_tests_PROGRAMS = sp65
sp65_SOURCES = init.c

dist_rtems_tests_DATA = sp65.scn
dist_rtems_tests_DATA += sp65.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp65_OBJECTS)
LINK_LIBS = $(sp65_LDLIBS)

sp65$(EXEEXT): $(sp65_OBJECTS) $(sp65_DEPENDENCIES)
	@rm -f sp65$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
