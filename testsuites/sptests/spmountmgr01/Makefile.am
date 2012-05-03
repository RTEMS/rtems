
rtems_tests_PROGRAMS = spmountmgr01
spmountmgr01_SOURCES = init.c

dist_rtems_tests_DATA = spmountmgr01.scn
dist_rtems_tests_DATA += spmountmgr01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spmountmgr01_OBJECTS)
LINK_LIBS = $(spmountmgr01_LDLIBS)

spmountmgr01$(EXEEXT): $(spmountmgr01_OBJECTS) $(spmountmgr01_DEPENDENCIES)
	@rm -f spmountmgr01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
