
rtems_tests_PROGRAMS = sp27a
sp27a_SOURCES = ../sp27/init.c

dist_rtems_tests_DATA = sp27a.scn
dist_rtems_tests_DATA += sp27a.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_COUNTING_SEMAPHORE

LINK_OBJS = $(sp27a_OBJECTS)
LINK_LIBS = $(sp27a_LDLIBS)

sp27a$(EXEEXT): $(sp27a_OBJECTS) $(sp27a_DEPENDENCIES)
	@rm -f sp27a$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
