MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond02
psxtmcond02_SOURCES  = init.c
psxtmcond02_SOURCES += ../../tmtests/include/timesys.h
psxtmcond02_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond02_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmcond02_OBJECTS) $(psxtmcond02_LDADD)
LINK_LIBS = $(psxtmcond02_LDLIBS)

psxtmcond02$(EXEEXT): $(psxtmcond02_OBJECTS) $(psxtmcond02_DEPENDENCIES)
	@rm -f psxtmcond02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
