
rtems_tests_PROGRAMS = sp73 
sp73_SOURCES = init.c

dist_rtems_tests_DATA = sp73.scn
dist_rtems_tests_DATA += sp73.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp73_OBJECTS)
LINK_LIBS = $(sp73_LDLIBS)

sp73$(EXEEXT): $(sp73_OBJECTS) $(sp73_DEPENDENCIES)
	@rm -f sp73$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
