
rtems_tests_PROGRAMS = tm15
tm15_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm15.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm15_OBJECTS)
LINK_LIBS = $(tm15_LDLIBS)

tm15$(EXEEXT): $(tm15_OBJECTS) $(tm15_DEPENDENCIES)
	@rm -f tm15$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
