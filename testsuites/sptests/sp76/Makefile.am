
rtems_tests_PROGRAMS = sp76
sp76_SOURCES = init.c

dist_rtems_tests_DATA = sp76.scn
dist_rtems_tests_DATA += sp76.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp76_OBJECTS)
LINK_LIBS = $(sp76_LDLIBS)

sp76$(EXEEXT): $(sp76_OBJECTS) $(sp76_DEPENDENCIES)
	@rm -f sp76$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
