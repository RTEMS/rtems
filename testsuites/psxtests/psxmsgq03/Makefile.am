
rtems_tests_PROGRAMS = psxmsgq03
psxmsgq03_SOURCES = init.c system.h ../include/pmacros.h \
     ../../support/src/test_support.c

dist_rtems_tests_DATA = psxmsgq03.scn
dist_rtems_tests_DATA += psxmsgq03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxmsgq03_OBJECTS)
LINK_LIBS = $(psxmsgq03_LDLIBS)

psxmsgq03$(EXEEXT): $(psxmsgq03_OBJECTS) $(psxmsgq03_DEPENDENCIES)
	@rm -f psxmsgq03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
