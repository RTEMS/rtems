MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond10
psxtmcond10_SOURCES  = ../psxtmcond08/init.c
psxtmcond10_SOURCES += ../../tmtests/include/timesys.h
psxtmcond10_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond10_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond10.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_TIMEDWAIT_WAIT_VALUE_IN_PAST

LINK_OBJS = $(psxtmcond10_OBJECTS) $(psxtmcond10_LDADD)
LINK_LIBS = $(psxtmcond10_LDLIBS)

psxtmcond10$(EXEEXT): $(psxtmcond10_OBJECTS) $(psxtmcond10_DEPENDENCIES)
	@rm -f psxtmcond10$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
