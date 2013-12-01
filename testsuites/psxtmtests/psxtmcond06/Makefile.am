MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond06
psxtmcond06_SOURCES  = init.c
psxtmcond06_SOURCES += ../../tmtests/include/timesys.h
psxtmcond06_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond06_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmcond06_OBJECTS) $(psxtmcond06_LDADD)
LINK_LIBS = $(psxtmcond06_LDLIBS)

psxtmcond06$(EXEEXT): $(psxtmcond06_OBJECTS) $(psxtmcond06_DEPENDENCIES)
	@rm -f psxtmcond06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
