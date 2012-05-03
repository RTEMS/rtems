
rtems_tests_PROGRAMS = tm12
tm12_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm12.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm12_OBJECTS)
LINK_LIBS = $(tm12_LDLIBS)

tm12$(EXEEXT): $(tm12_OBJECTS) $(tm12_DEPENDENCIES)
	@rm -f tm12$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
