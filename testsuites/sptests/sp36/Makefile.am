
rtems_tests_PROGRAMS = sp36
sp36_SOURCES = strict_order_mut.c

dist_rtems_tests_DATA = sp36.scn
dist_rtems_tests_DATA += sp36.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp36_OBJECTS)
LINK_LIBS = $(sp36_LDLIBS)

sp36$(EXEEXT): $(sp36_OBJECTS) $(sp36_DEPENDENCIES)
	@rm -f sp36$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
