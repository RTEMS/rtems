MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond07
psxtmcond07_SOURCES  = init.c
psxtmcond07_SOURCES += ../../tmtests/include/timesys.h
psxtmcond07_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond07_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmcond07_OBJECTS) $(psxtmcond07_LDADD)
LINK_LIBS = $(psxtmcond07_LDLIBS)

psxtmcond07$(EXEEXT): $(psxtmcond07_OBJECTS) $(psxtmcond07_DEPENDENCIES)
	@rm -f psxtmcond07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
