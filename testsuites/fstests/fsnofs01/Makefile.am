rtems_tests_PROGRAMS = fsnofs01
fsnofs01_SOURCES = init.c

dist_rtems_tests_DATA = fsnofs01.scn fsnofs01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsnofs01_OBJECTS)
LINK_LIBS = $(fsnofs01_LDLIBS)

fsnofs01$(EXEEXT): $(fsnofs01_OBJECTS) $(fsnofs01_DEPENDENCIES)
	@rm -f fsnofs01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
