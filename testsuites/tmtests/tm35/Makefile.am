
rtems_tests_PROGRAMS = tm35
tm35_SOURCES = ../tm02/task1.c
tm35_SOURCES += ../tm02/system.h
tm35_SOURCES += ../include/timesys.h
tm35_SOURCES += ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm35.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../tm02
AM_CPPFLAGS += -DTM35

LINK_OBJS = $(tm35_OBJECTS)
LINK_LIBS = $(tm35_LDLIBS)

tm35$(EXEEXT): $(tm35_OBJECTS) $(tm35_DEPENDENCIES)
	@rm -f tm35$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
