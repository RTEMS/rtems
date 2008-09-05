##
##  $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = base_mp-node1.exe
base_mp_node1_exe_SOURCES = ../init.c ../apptask.c ../system.h

dist_rtems_tests_DATA = base_mp-node1.scn
dist_rtems_tests_DATA += base_mp-node1.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

base_mp_node1_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -DNODE_NUMBER=1 -I.

LINK_OBJS = $(base_mp_node1_exe_OBJECTS) $(base_mp_node1_exe_LDADD)
LINK_LIBS = $(base_mp_node1_exe_LDLIBS)

base_mp-node1.exe$(EXEEXT): $(base_mp_node1_exe_OBJECTS) $(base_mp_node1_exe_DEPENDENCIES)
	@rm -f base_mp-node1.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
