
rtems_tests_PROGRAMS = sp31
sp31_SOURCES = delay.c init.c prtime.c task1.c system.h

dist_rtems_tests_DATA = sp31.scn
dist_rtems_tests_DATA += sp31.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp31_OBJECTS)
LINK_LIBS = $(sp31_LDLIBS)

sp31$(EXEEXT): $(sp31_OBJECTS) $(sp31_DEPENDENCIES)
	@rm -f sp31$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
