
rtems_tests_PROGRAMS = sp74 
sp74_SOURCES = init.c

dist_rtems_tests_DATA = sp74.scn
dist_rtems_tests_DATA += sp74.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp74_OBJECTS)
LINK_LIBS = $(sp74_LDLIBS)

sp74$(EXEEXT): $(sp74_OBJECTS) $(sp74_DEPENDENCIES)
	@rm -f sp74$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
