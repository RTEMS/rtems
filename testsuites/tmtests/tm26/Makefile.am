
rtems_tests_PROGRAMS = tm26
tm26_SOURCES = task1.c system.h fptest.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm26.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm26_OBJECTS)
LINK_LIBS = $(tm26_LDLIBS)

tm26$(EXEEXT): $(tm26_OBJECTS) $(tm26_DEPENDENCIES)
	@rm -f tm26$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
