
rtems_tests_PROGRAMS = tm17
tm17_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm17.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm17_OBJECTS)
LINK_LIBS = $(tm17_LDLIBS)

tm17$(EXEEXT): $(tm17_OBJECTS) $(tm17_DEPENDENCIES)
	@rm -f tm17$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
