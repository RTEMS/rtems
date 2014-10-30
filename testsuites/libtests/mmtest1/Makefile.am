##
##  $Id: Makefile.am
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = mmtest1
mmtest1_SOURCES = init.c system.h 

dist_rtems_tests_DATA = mmtests.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mmtest1_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(mmtest1_OBJECTS) $(mmtest1_LDADD)
LINK_LIBS = $(mmtest1_LDLIBS)

mmtest1$(EXEEXT): $(mmtest1_OBJECTS) $(mmtest1_DEPENDENCIES)
	  @rm -f mmtest1$(EXEEXT)
	  $(make-exe)

include $(top_srcdir)/../automake/local.am

