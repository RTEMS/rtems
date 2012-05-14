
rtems_tests_PROGRAMS = tm01
tm01_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm01_OBJECTS)
LINK_LIBS = $(tm01_LDLIBS)

tm01$(EXEEXT): $(tm01_OBJECTS) $(tm01_DEPENDENCIES)
	@rm -f tm01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
