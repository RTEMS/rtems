rtems_tests_PROGRAMS = mp14-node2
mp14_node2_SOURCES = ../exit.c ../init.c ../delay.c ../evtask1.c ../evtmtask.c \
    ../pttask1.c ../smtask1.c ../msgtask1.c ../system.h

dist_rtems_tests_DATA = mp14-node2.scn
dist_rtems_tests_DATA += mp14-node2.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -DNODE_NUMBER=2 -I$(srcdir)/..
AM_CPPFLAGS += -DMAX_LONG_TEST_DURATION=$(MAX_LONG_TEST_DURATION)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mp14_node2_OBJECTS)
LINK_LIBS = $(mp14_node2_LDLIBS)

mp14-node2$(EXEEXT): $(mp14_node2_OBJECTS) $(mp14_node2_DEPENDENCIES)
	@rm -f mp14-node2$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
