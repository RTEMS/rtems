
rtems_tests_PROGRAMS = sp17
sp17_SOURCES = init.c asr.c task1.c task2.c system.h

dist_rtems_tests_DATA = sp17.scn
dist_rtems_tests_DATA += sp17.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp17_OBJECTS)
LINK_LIBS = $(sp17_LDLIBS)

sp17$(EXEEXT): $(sp17_OBJECTS) $(sp17_DEPENDENCIES)
	@rm -f sp17$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
