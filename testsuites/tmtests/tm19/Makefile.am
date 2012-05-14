
rtems_tests_PROGRAMS = tm19
tm19_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm19.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm19_OBJECTS)
LINK_LIBS = $(tm19_LDLIBS)

tm19$(EXEEXT): $(tm19_OBJECTS) $(tm19_DEPENDENCIES)
	@rm -f tm19$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
