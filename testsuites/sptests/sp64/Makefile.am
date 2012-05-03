
rtems_tests_PROGRAMS = sp64
sp64_SOURCES = init.c

dist_rtems_tests_DATA = sp64.scn
dist_rtems_tests_DATA += sp64.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp64_OBJECTS)
LINK_LIBS = $(sp64_LDLIBS)

sp64$(EXEEXT): $(sp64_OBJECTS) $(sp64_DEPENDENCIES)
	@rm -f sp64$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
