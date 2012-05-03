
rtems_tests_PROGRAMS = tm14
tm14_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm14.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm14_OBJECTS)
LINK_LIBS = $(tm14_LDLIBS)

tm14$(EXEEXT): $(tm14_OBJECTS) $(tm14_DEPENDENCIES)
	@rm -f tm14$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
