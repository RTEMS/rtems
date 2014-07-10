
rtems_tests_PROGRAMS = tm34
tm34_SOURCES = ../tm03/task1.c
tm34_SOURCES += ../tm03/system.h
tm34_SOURCES += ../include/timesys.h
tm34_SOURCES += ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm34.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../tm03
AM_CPPFLAGS += -DTM34

LINK_OBJS = $(tm34_OBJECTS)
LINK_LIBS = $(tm34_LDLIBS)

tm34$(EXEEXT): $(tm34_OBJECTS) $(tm34_DEPENDENCIES)
	@rm -f tm34$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
