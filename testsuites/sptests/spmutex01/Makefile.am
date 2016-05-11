rtems_tests_PROGRAMS = spmutex01
spmutex01_SOURCES = init.c

dist_rtems_tests_DATA = spmutex01.scn spmutex01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spmutex01_OBJECTS)
LINK_LIBS = $(spmutex01_LDLIBS)

spmutex01$(EXEEXT): $(spmutex01_OBJECTS) $(spmutex01_DEPENDENCIES)
	@rm -f spmutex01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
