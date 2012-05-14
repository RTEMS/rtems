
rtems_tests_PROGRAMS = sp14
sp14_SOURCES = init.c asr.c task1.c task2.c system.h

dist_rtems_tests_DATA = sp14.scn
dist_rtems_tests_DATA += sp14.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp14_OBJECTS)
LINK_LIBS = $(sp14_LDLIBS)

sp14$(EXEEXT): $(sp14_OBJECTS) $(sp14_DEPENDENCIES)
	@rm -f sp14$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
