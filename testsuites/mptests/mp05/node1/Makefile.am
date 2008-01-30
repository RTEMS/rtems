##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = mp05-node1.exe
mp05_node1_exe_SOURCES = ../init.c ../asr.c ../task1.c ../system.h

dist_rtems_tests_DATA = mp05-node1.scn
dist_rtems_tests_DATA += mp05-node1.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mp05_node1_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -DNODE_NUMBER=1 -I$(srcdir)/..

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mp05_node1_exe_OBJECTS) $(mp05_node1_exe_LDADD)
LINK_LIBS = $(mp05_node1_exe_LDLIBS)

mp05-node1.exe$(EXEEXT): $(mp05_node1_exe_OBJECTS) $(mp05_node1_exe_DEPENDENCIES)
	@rm -f mp05-node1.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
