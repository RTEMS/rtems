
rtems_tests_PROGRAMS = psxtmrwlock07
psxtmrwlock07_SOURCES  = init.c
psxtmrwlock07_SOURCES += ../../tmtests/include/timesys.h
psxtmrwlock07_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmrwlock07_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmrwlock07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmrwlock07_OBJECTS)
LINK_LIBS = $(psxtmrwlock07_LDLIBS)

psxtmrwlock07$(EXEEXT): $(psxtmrwlock07_OBJECTS) $(psxtmrwlock07_DEPENDENCIES)
	@rm -f psxtmrwlock07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
