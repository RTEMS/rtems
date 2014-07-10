
rtems_tests_PROGRAMS = tm33
tm33_SOURCES = ../tm02/task1.c
tm33_SOURCES += ../tm02/system.h
tm33_SOURCES += ../include/timesys.h
tm33_SOURCES += ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm33.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../tm02
AM_CPPFLAGS += -DTM33

LINK_OBJS = $(tm33_OBJECTS)
LINK_LIBS = $(tm33_LDLIBS)

tm33$(EXEEXT): $(tm33_OBJECTS) $(tm33_DEPENDENCIES)
	@rm -f tm33$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
