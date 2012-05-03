
rtems_tests_PROGRAMS = sp16
sp16_SOURCES = init.c task1.c task2.c task3.c task4.c task5.c system.h

dist_rtems_tests_DATA = sp16.scn
dist_rtems_tests_DATA += sp16.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp16_OBJECTS)
LINK_LIBS = $(sp16_LDLIBS)

sp16$(EXEEXT): $(sp16_OBJECTS) $(sp16_DEPENDENCIES)
	@rm -f sp16$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
