
rtems_tests_PROGRAMS = spsimplesched02 
spsimplesched02_SOURCES = init.c

dist_rtems_tests_DATA = spsimplesched02.scn
dist_rtems_tests_DATA += spsimplesched02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsimplesched02_OBJECTS)
LINK_LIBS = $(spsimplesched02_LDLIBS)

spsimplesched02$(EXEEXT): $(spsimplesched02_OBJECTS) $(spsimplesched02_DEPENDENCIES)
	@rm -f spsimplesched02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
