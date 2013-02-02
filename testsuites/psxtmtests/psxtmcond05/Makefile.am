MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond05
psxtmcond05_SOURCES  = init.c
psxtmcond05_SOURCES += ../../tmtests/include/timesys.h
psxtmcond05_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond05_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmcond05_OBJECTS) $(psxtmcond05_LDADD)
LINK_LIBS = $(psxtmcond05_LDLIBS)

psxtmcond05$(EXEEXT): $(psxtmcond05_OBJECTS) $(psxtmcond05_DEPENDENCIES)
	@rm -f psxtmcond05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
