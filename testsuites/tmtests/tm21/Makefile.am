
rtems_tests_PROGRAMS = tm21
tm21_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm21.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm21_OBJECTS)
LINK_LIBS = $(tm21_LDLIBS)

tm21$(EXEEXT): $(tm21_OBJECTS) $(tm21_DEPENDENCIES)
	@rm -f tm21$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
