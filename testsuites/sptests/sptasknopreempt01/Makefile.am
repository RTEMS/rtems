rtems_tests_PROGRAMS = sptasknopreempt01
sptasknopreempt01_SOURCES = init.c

dist_rtems_tests_DATA = sptasknopreempt01.scn sptasknopreempt01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptasknopreempt01_OBJECTS)
LINK_LIBS = $(sptasknopreempt01_LDLIBS)

sptasknopreempt01$(EXEEXT): $(sptasknopreempt01_OBJECTS) $(sptasknopreempt01_DEPENDENCIES)
	@rm -f sptasknopreempt01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
