
rtems_tests_PROGRAMS = tm13
tm13_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm13.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm13_OBJECTS)
LINK_LIBS = $(tm13_LDLIBS)

tm13$(EXEEXT): $(tm13_OBJECTS) $(tm13_DEPENDENCIES)
	@rm -f tm13$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
