
rtems_tests_PROGRAMS = sp02
sp02_SOURCES = init.c task1.c task2.c task3.c preempt.c system.h

dist_rtems_tests_DATA = sp02.scn
dist_rtems_tests_DATA += sp02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp02_OBJECTS)
LINK_LIBS = $(sp02_LDLIBS)

sp02$(EXEEXT): $(sp02_OBJECTS) $(sp02_DEPENDENCIES)
	@rm -f sp02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
