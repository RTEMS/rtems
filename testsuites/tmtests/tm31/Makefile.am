
rtems_tests_PROGRAMS = tm31
tm31_SOURCES = ../tm02/task1.c
tm31_SOURCES += ../tm02/system.h
tm31_SOURCES += ../include/timesys.h
tm31_SOURCES += ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm31.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../tm02
AM_CPPFLAGS += -DTM31

LINK_OBJS = $(tm31_OBJECTS)
LINK_LIBS = $(tm31_LDLIBS)

tm31$(EXEEXT): $(tm31_OBJECTS) $(tm31_DEPENDENCIES)
	@rm -f tm31$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
