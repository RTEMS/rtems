
rtems_tests_PROGRAMS = sptask_err01
sptask_err01_SOURCES = init.c task2.c system.h

dist_rtems_tests_DATA = sptask_err01.scn
dist_rtems_tests_DATA += sptask_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptask_err01_OBJECTS)
LINK_LIBS = $(sptask_err01_LDLIBS)

sptask_err01$(EXEEXT): $(sptask_err01_OBJECTS) $(sptask_err01_DEPENDENCIES)
	@rm -f sptask_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
