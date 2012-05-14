
rtems_tests_PROGRAMS = sp45
sp45_SOURCES = init.c

dist_rtems_tests_DATA = sp45.scn
dist_rtems_tests_DATA += sp45.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp45_OBJECTS)
LINK_LIBS = $(sp45_LDLIBS)

sp45$(EXEEXT): $(sp45_OBJECTS) $(sp45_DEPENDENCIES)
	@rm -f sp45$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
