rtems_tests_PROGRAMS = syscall01
syscall01_SOURCES = init.c

dist_rtems_tests_DATA = syscall01.scn syscall01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(syscall01_OBJECTS)
LINK_LIBS = $(syscall01_LDLIBS)

syscall01$(EXEEXT): $(syscall01_OBJECTS) $(syscall01_DEPENDENCIES)
	@rm -f syscall01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
