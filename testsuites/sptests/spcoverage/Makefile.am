
rtems_tests_PROGRAMS = spcoverage
spcoverage_SOURCES = init.c

dist_rtems_tests_DATA = spcoverage.scn
dist_rtems_tests_DATA += spcoverage.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spcoverage_OBJECTS)
LINK_LIBS = $(spcoverage_LDLIBS)

spcoverage$(EXEEXT): $(spcoverage_OBJECTS) $(spcoverage_DEPENDENCIES)
	@rm -f spcoverage$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
