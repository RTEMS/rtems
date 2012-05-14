
rtems_tests_PROGRAMS = tmoverhd
tmoverhd_SOURCES = testtask.c empty.c system.h dumrtems.h \
    ../include/timesys.h ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tmoverhd.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tmoverhd_OBJECTS)
LINK_LIBS = $(tmoverhd_LDLIBS)

tmoverhd$(EXEEXT): $(tmoverhd_OBJECTS) $(tmoverhd_DEPENDENCIES)
	@rm -f tmoverhd$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
