
rtems_tests_PROGRAMS = sp19
sp19_SOURCES = init.c first.c fptask.c task1.c system.h fptest.h inttest.h

dist_rtems_tests_DATA = sp19.scn
dist_rtems_tests_DATA += sp19.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp19_OBJECTS)
LINK_LIBS = $(sp19_LDLIBS)

sp19$(EXEEXT): $(sp19_OBJECTS) $(sp19_DEPENDENCIES)
	@rm -f sp19$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
