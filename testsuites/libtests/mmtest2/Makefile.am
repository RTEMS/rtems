##
##  $Id: Makefile.am
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = mmtest2
mmtest2_SOURCES = init.c system.h 

dist_rtems_tests_DATA = mmtests.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mmtest2_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(mmtest2_OBJECTS) $(mmtest2_LDADD)
LINK_LIBS = $(mmtest2_LDLIBS)

mmtest2$(EXEEXT): $(mmtest2_OBJECTS) $(mmtest2_DEPENDENCIES)
	  @rm -f mmtest2$(EXEEXT)
	  $(make-exe)

include $(top_srcdir)/../automake/local.am

