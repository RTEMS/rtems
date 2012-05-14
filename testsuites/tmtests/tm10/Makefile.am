
rtems_tests_PROGRAMS = tm10
tm10_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm10.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm10_OBJECTS)
LINK_LIBS = $(tm10_LDLIBS)

tm10$(EXEEXT): $(tm10_OBJECTS) $(tm10_DEPENDENCIES)
	@rm -f tm10$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
