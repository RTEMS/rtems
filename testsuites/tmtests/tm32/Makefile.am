
rtems_tests_PROGRAMS = tm32
tm32_SOURCES = ../tm03/task1.c
tm32_SOURCES += ../tm03/system.h
tm32_SOURCES += ../include/timesys.h
tm32_SOURCES += ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm32.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../tm03
AM_CPPFLAGS += -DTM32

LINK_OBJS = $(tm32_OBJECTS)
LINK_LIBS = $(tm32_LDLIBS)

tm32$(EXEEXT): $(tm32_OBJECTS) $(tm32_DEPENDENCIES)
	@rm -f tm32$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
