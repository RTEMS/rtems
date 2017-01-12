rtems_tests_PROGRAMS = psxthreadname01
psxthreadname01_SOURCES = init.c

dist_rtems_tests_DATA = psxthreadname01.scn psxthreadname01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxthreadname01_OBJECTS)
LINK_LIBS = $(psxthreadname01_LDLIBS)

psxthreadname01$(EXEEXT): $(psxthreadname01_OBJECTS) $(psxthreadname01_DEPENDENCIES)
	@rm -f psxthreadname01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
