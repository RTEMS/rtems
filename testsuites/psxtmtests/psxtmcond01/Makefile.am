MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond01
psxtmcond01_SOURCES  = init.c
psxtmcond01_SOURCES += ../../tmtests/include/timesys.h
psxtmcond01_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond01_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmcond01_OBJECTS) $(psxtmcond01_LDADD)
LINK_LIBS = $(psxtmcond01_LDLIBS)

psxtmcond01$(EXEEXT): $(psxtmcond01_OBJECTS) $(psxtmcond01_DEPENDENCIES)
	@rm -f psxtmcond01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am

