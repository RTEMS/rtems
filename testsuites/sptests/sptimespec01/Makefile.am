
rtems_tests_PROGRAMS = sptimespec01
sptimespec01_SOURCES = init.c

dist_rtems_tests_DATA = sptimespec01.scn
dist_rtems_tests_DATA += sptimespec01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sptimespec01_OBJECTS) $(sptimespec01_LDADD)
LINK_LIBS = $(sptimespec01_LDLIBS)

sptimespec01$(EXEEXT): $(sptimespec01_OBJECTS) $(sptimespec01_DEPENDENCIES)
	@rm -f sptimespec01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
