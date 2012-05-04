rtems_tests_PROGRAMS = mp09-node1
mp09_node1_SOURCES = ../init.c ../task1.c ../recvmsg.c ../sendmsg.c ../system.h

dist_rtems_tests_DATA = mp09-node1.scn
dist_rtems_tests_DATA += mp09-node1.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -DNODE_NUMBER=1 -I$(srcdir)/..

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mp09_node1_OBJECTS)
LINK_LIBS = $(mp09_node1_LDLIBS)

mp09-node1$(EXEEXT): $(mp09_node1_OBJECTS) $(mp09_node1_DEPENDENCIES)
	@rm -f mp09-node1$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
