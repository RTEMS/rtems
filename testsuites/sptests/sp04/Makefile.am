
rtems_tests_PROGRAMS = sp04
sp04_SOURCES = init.c task1.c task2.c task3.c tswitch.c system.h

dist_rtems_tests_DATA = sp04.scn
dist_rtems_tests_DATA += sp04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp04_OBJECTS)
LINK_LIBS = $(sp04_LDLIBS)

sp04$(EXEEXT): $(sp04_OBJECTS) $(sp04_DEPENDENCIES)
	@rm -f sp04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
