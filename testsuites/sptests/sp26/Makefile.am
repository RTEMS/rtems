
rtems_tests_PROGRAMS = sp26
sp26_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp26.scn
dist_rtems_tests_DATA += sp26.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp26_OBJECTS)
LINK_LIBS = $(sp26_LDLIBS)

sp26$(EXEEXT): $(sp26_OBJECTS) $(sp26_DEPENDENCIES)
	@rm -f sp26$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
