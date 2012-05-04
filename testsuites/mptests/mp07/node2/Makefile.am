rtems_tests_PROGRAMS = mp07-node2
mp07_node2_SOURCES = ../init.c ../task1.c ../system.h

dist_rtems_tests_DATA = mp07-node2.scn
dist_rtems_tests_DATA += mp07-node2.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -DNODE_NUMBER=2 -I$(srcdir)/..

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mp07_node2_OBJECTS)
LINK_LIBS = $(mp07_node2_LDLIBS)

mp07-node2$(EXEEXT): $(mp07_node2_OBJECTS) $(mp07_node2_DEPENDENCIES)
	@rm -f mp07-node2$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
