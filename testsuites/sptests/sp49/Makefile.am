
rtems_tests_PROGRAMS = sp49
sp49_SOURCES = init.c

dist_rtems_tests_DATA = sp49.scn
dist_rtems_tests_DATA += sp49.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp49_OBJECTS)
LINK_LIBS = $(sp49_LDLIBS)

sp49$(EXEEXT): $(sp49_OBJECTS) $(sp49_DEPENDENCIES)
	@rm -f sp49$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
