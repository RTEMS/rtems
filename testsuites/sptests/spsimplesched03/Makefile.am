
rtems_tests_PROGRAMS = spsimplesched03 
spsimplesched03_SOURCES = init.c

dist_rtems_tests_DATA = spsimplesched03.scn
dist_rtems_tests_DATA += spsimplesched03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsimplesched03_OBJECTS)
LINK_LIBS = $(spsimplesched03_LDLIBS)

spsimplesched03$(EXEEXT): $(spsimplesched03_OBJECTS) $(spsimplesched03_DEPENDENCIES)
	@rm -f spsimplesched03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
