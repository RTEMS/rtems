rtems_tests_PROGRAMS = fsdosfsformat01
fsdosfsformat01_SOURCES = init.c

dist_rtems_tests_DATA = fsdosfsformat01.scn fsdosfsformat01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsdosfsformat01_OBJECTS)
LINK_LIBS = $(fsdosfsformat01_LDLIBS)

fsdosfsformat01$(EXEEXT): $(fsdosfsformat01_OBJECTS) $(fsdosfsformat01_DEPENDENCIES)
	@rm -f fsdosfsformat01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
