
rtems_tests_PROGRAMS = tm20
tm20_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm20.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm20_OBJECTS)
LINK_LIBS = $(tm20_LDLIBS)

tm20$(EXEEXT): $(tm20_OBJECTS) $(tm20_DEPENDENCIES)
	@rm -f tm20$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
