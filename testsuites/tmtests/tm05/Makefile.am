
rtems_tests_PROGRAMS = tm05
tm05_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm05_OBJECTS)
LINK_LIBS = $(tm05_LDLIBS)

tm05$(EXEEXT): $(tm05_OBJECTS) $(tm05_DEPENDENCIES)
	@rm -f tm05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
