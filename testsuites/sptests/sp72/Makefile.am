
rtems_tests_PROGRAMS = sp72
sp72_SOURCES = init.c

dist_rtems_tests_DATA = sp72.scn
dist_rtems_tests_DATA += sp72.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp72_OBJECTS)
LINK_LIBS = $(sp72_LDLIBS)

sp72$(EXEEXT): $(sp72_OBJECTS) $(sp72_DEPENDENCIES)
	@rm -f sp72$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
