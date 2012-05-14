MANAGERS = io rate_monotonic semaphore clock

rtems_tests_PROGRAMS = spqreslib
spqreslib_SOURCES = init.c task_periodic.c system.h

dist_rtems_tests_DATA = spqreslib.scn
dist_rtems_tests_DATA += spqreslib.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spqreslib_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spqreslib_OBJECTS) $(spqreslib_LDADD)
LINK_LIBS = $(spqreslib_LDLIBS)

spqreslib$(EXEEXT): $(spqreslib_OBJECTS) $(spqreslib_DEPENDENCIES)
	@rm -f spqreslib$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
