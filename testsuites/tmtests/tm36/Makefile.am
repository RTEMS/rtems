
rtems_tests_PROGRAMS = tm36
tm36_SOURCES = ../tm03/task1.c
tm36_SOURCES += ../tm03/system.h
tm36_SOURCES += ../include/timesys.h
tm36_SOURCES += ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm36.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../tm03
AM_CPPFLAGS += -DTM36

LINK_OBJS = $(tm36_OBJECTS)
LINK_LIBS = $(tm36_LDLIBS)

tm36$(EXEEXT): $(tm36_OBJECTS) $(tm36_DEPENDENCIES)
	@rm -f tm36$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
