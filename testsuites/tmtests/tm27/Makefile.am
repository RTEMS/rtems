
rtems_tests_PROGRAMS = tm27
tm27_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm27.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm27_OBJECTS)
LINK_LIBS = $(tm27_LDLIBS)

tm27$(EXEEXT): $(tm27_OBJECTS) $(tm27_DEPENDENCIES)
	@rm -f tm27$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
