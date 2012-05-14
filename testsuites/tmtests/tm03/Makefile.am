
rtems_tests_PROGRAMS = tm03
tm03_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm03_OBJECTS)
LINK_LIBS = $(tm03_LDLIBS)

tm03$(EXEEXT): $(tm03_OBJECTS) $(tm03_DEPENDENCIES)
	@rm -f tm03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
