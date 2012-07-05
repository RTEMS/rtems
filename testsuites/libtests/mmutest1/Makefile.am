##
##  $Id: Makefile.am
##

MANAGERS = io semaphore

rtems_tests_PROGRAMS = mmutest1
mmutest1_SOURCES = init.c system.h 

dist_rtems_tests_DATA = mmutests.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mmutest1_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(mmutest1_OBJECTS) $(mmutest1_LDADD)
LINK_LIBS = $(mmutest1_LDLIBS)

mmutest1$(EXEEXT): $(mmutest1_OBJECTS) $(mmutest1_DEPENDENCIES)
	  @rm -f mmutest1$(EXEEXT)
	  $(make-exe)

include $(top_srcdir)/../automake/local.am

