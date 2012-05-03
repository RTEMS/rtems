
rtems_tests_PROGRAMS = sp15
sp15_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sp15.scn
dist_rtems_tests_DATA += sp15.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp15_OBJECTS)
LINK_LIBS = $(sp15_LDLIBS)

sp15$(EXEEXT): $(sp15_OBJECTS) $(sp15_DEPENDENCIES)
	@rm -f sp15$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
