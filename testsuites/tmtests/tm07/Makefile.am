
rtems_tests_PROGRAMS = tm07
tm07_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm07_OBJECTS)
LINK_LIBS = $(tm07_LDLIBS)

tm07$(EXEEXT): $(tm07_OBJECTS) $(tm07_DEPENDENCIES)
	@rm -f tm07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
