
rtems_tests_PROGRAMS = tm16
tm16_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm16.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm16_OBJECTS)
LINK_LIBS = $(tm16_LDLIBS)

tm16$(EXEEXT): $(tm16_OBJECTS) $(tm16_DEPENDENCIES)
	@rm -f tm16$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
