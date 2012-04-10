rtems_tests_PROGRAMS = rbheap01
rbheap01_SOURCES = init.c

dist_rtems_tests_DATA = rbheap01.scn rbheap01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rbheap01_OBJECTS)
LINK_LIBS = $(rbheap01_LDLIBS)

rbheap01$(EXEEXT): $(rbheap01_OBJECTS) $(rbheap01_DEPENDENCIES)
	@rm -f rbheap01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
