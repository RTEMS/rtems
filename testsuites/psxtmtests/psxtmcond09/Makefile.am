MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond09
psxtmcond09_SOURCES  = ../psxtmcond08/init.c
psxtmcond09_SOURCES += ../../tmtests/include/timesys.h
psxtmcond09_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond09_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_TIMEDWAIT_WITH_VALUE

LINK_OBJS = $(psxtmcond09_OBJECTS) $(psxtmcond09_LDADD)
LINK_LIBS = $(psxtmcond09_LDLIBS)

psxtmcond09$(EXEEXT): $(psxtmcond09_OBJECTS) $(psxtmcond09_DEPENDENCIES)
	@rm -f psxtmcond09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
