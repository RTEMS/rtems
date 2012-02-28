rtems_tests_PROGRAMS = fsbdpart01
fsbdpart01_SOURCES = init.c

dist_rtems_tests_DATA = fsbdpart01.scn fsbdpart01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsbdpart01_OBJECTS)
LINK_LIBS = $(fsbdpart01_LDLIBS)

fsbdpart01$(EXEEXT): $(fsbdpart01_OBJECTS) $(fsbdpart01_DEPENDENCIES)
	@rm -f fsbdpart01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
