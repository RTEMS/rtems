rtems_tests_PROGRAMS = spsysinit01
spsysinit01_SOURCES = init.c

dist_rtems_tests_DATA = spsysinit01.scn spsysinit01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsysinit01_OBJECTS)
LINK_LIBS = $(spsysinit01_LDLIBS)

spsysinit01$(EXEEXT): $(spsysinit01_OBJECTS) $(spsysinit01_DEPENDENCIES)
	@rm -f spsysinit01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
