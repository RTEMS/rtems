
rtems_tests_PROGRAMS = sp29
sp29_SOURCES = init.c

dist_rtems_tests_DATA = sp29.scn
dist_rtems_tests_DATA += sp29.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp29_OBJECTS)
LINK_LIBS = $(sp29_LDLIBS)

sp29$(EXEEXT): $(sp29_OBJECTS) $(sp29_DEPENDENCIES)
	@rm -f sp29$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
