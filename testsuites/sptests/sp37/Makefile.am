
rtems_tests_PROGRAMS = sp37
sp37_SOURCES = init.c system.h

dist_rtems_tests_DATA = sp37.scn
dist_rtems_tests_DATA += sp37.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp37_OBJECTS)
LINK_LIBS = $(sp37_LDLIBS)

sp37$(EXEEXT): $(sp37_OBJECTS) $(sp37_DEPENDENCIES)
	@rm -f sp37$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
