
rtems_tests_PROGRAMS = tm02
tm02_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm02_OBJECTS)
LINK_LIBS = $(tm02_LDLIBS)

tm02$(EXEEXT): $(tm02_OBJECTS) $(tm02_DEPENDENCIES)
	@rm -f tm02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
