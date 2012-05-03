
rtems_tests_PROGRAMS = sp13
sp13_SOURCES = init.c fillbuff.c putbuff.c task1.c task2.c task3.c system.h

dist_rtems_tests_DATA = sp13.scn
dist_rtems_tests_DATA += sp13.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp13_OBJECTS)
LINK_LIBS = $(sp13_LDLIBS)

sp13$(EXEEXT): $(sp13_OBJECTS) $(sp13_DEPENDENCIES)
	@rm -f sp13$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
