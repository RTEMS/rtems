rtems_tests_PROGRAMS = spsyslock01
spsyslock01_SOURCES = init.c

dist_rtems_tests_DATA = spsyslock01.scn spsyslock01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsyslock01_OBJECTS)
LINK_LIBS = $(spsyslock01_LDLIBS)

spsyslock01$(EXEEXT): $(spsyslock01_OBJECTS) $(spsyslock01_DEPENDENCIES)
	@rm -f spsyslock01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
