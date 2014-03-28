
rtems_tests_PROGRAMS = sptask_err04
sptask_err04_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = sptask_err04.scn
dist_rtems_tests_DATA += sptask_err04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptask_err04_OBJECTS)
LINK_LIBS = $(sptask_err04_LDLIBS)

sptask_err04$(EXEEXT): $(sptask_err04_OBJECTS) $(sptask_err04_DEPENDENCIES)
	@rm -f sptask_err04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
