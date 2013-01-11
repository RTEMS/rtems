MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond03
psxtmcond03_SOURCES  = init.c
psxtmcond03_SOURCES += ../../tmtests/include/timesys.h
psxtmcond03_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond03_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmcond03_OBJECTS) $(psxtmcond03_LDADD)
LINK_LIBS = $(psxtmcond03_LDLIBS)

psxtmcond03$(EXEEXT): $(psxtmcond03_OBJECTS) $(psxtmcond03_DEPENDENCIES)
	@rm -f psxtmcond03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
