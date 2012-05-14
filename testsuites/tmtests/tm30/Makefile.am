
rtems_tests_PROGRAMS = tm30
tm30_SOURCES = init.c ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = tm30.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm30_OBJECTS)
LINK_LIBS = $(tm30_LDLIBS)

tm30$(EXEEXT): $(tm30_OBJECTS) $(tm30_DEPENDENCIES)
	@rm -f tm30$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
