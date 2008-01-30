##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = mp01-node2.exe
mp01_node2_exe_SOURCES = ../init.c ../task1.c ../system.h

dist_rtems_tests_DATA = mp01-node2.scn
dist_rtems_tests_DATA += mp01-node2.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mp01_node2_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -DNODE_NUMBER=2 -I$(srcdir)/..

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mp01_node2_exe_OBJECTS) $(mp01_node2_exe_LDADD)
LINK_LIBS = $(mp01_node2_exe_LDLIBS)

mp01-node2.exe$(EXEEXT): $(mp01_node2_exe_OBJECTS) $(mp01_node2_exe_DEPENDENCIES)
	@rm -f mp01-node2.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
