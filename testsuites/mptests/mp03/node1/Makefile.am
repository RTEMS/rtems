##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = mp03-node1.exe
mp03_node1_exe_SOURCES = ../init.c ../task1.c ../delay.c ../system.h

dist_rtems_tests_DATA = mp03-node1.scn
dist_rtems_tests_DATA += mp03-node1.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mp03_node1_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -DNODE_NUMBER=1 -I$(srcdir)/..

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mp03_node1_exe_OBJECTS) $(mp03_node1_exe_LDADD)
LINK_LIBS = $(mp03_node1_exe_LDLIBS)

mp03-node1.exe$(EXEEXT): $(mp03_node1_exe_OBJECTS) $(mp03_node1_exe_DEPENDENCIES)
	@rm -f mp03-node1.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
