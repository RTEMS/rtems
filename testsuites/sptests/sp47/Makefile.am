
rtems_tests_PROGRAMS = sp47
sp47_SOURCES = init.c

dist_rtems_tests_DATA = sp47.scn
dist_rtems_tests_DATA += sp47.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp47_OBJECTS)
LINK_LIBS = $(sp47_LDLIBS)

sp47$(EXEEXT): $(sp47_OBJECTS) $(sp47_DEPENDENCIES)
	@rm -f sp47$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
