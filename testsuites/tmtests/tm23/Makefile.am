
rtems_tests_PROGRAMS = tm23
tm23_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm23.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm23_OBJECTS)
LINK_LIBS = $(tm23_LDLIBS)

tm23$(EXEEXT): $(tm23_OBJECTS) $(tm23_DEPENDENCIES)
	@rm -f tm23$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
