
rtems_tests_PROGRAMS = tm06
tm06_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm06_OBJECTS)
LINK_LIBS = $(tm06_LDLIBS)

tm06$(EXEEXT): $(tm06_OBJECTS) $(tm06_DEPENDENCIES)
	@rm -f tm06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
