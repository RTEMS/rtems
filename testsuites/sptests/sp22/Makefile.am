
rtems_tests_PROGRAMS = sp22
sp22_SOURCES = init.c prtime.c delay.c task1.c system.h

dist_rtems_tests_DATA = sp22.scn
dist_rtems_tests_DATA += sp22.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp22_OBJECTS)
LINK_LIBS = $(sp22_LDLIBS)

sp22$(EXEEXT): $(sp22_OBJECTS) $(sp22_DEPENDENCIES)
	@rm -f sp22$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
