MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond08
psxtmcond08_SOURCES  = init.c
psxtmcond08_SOURCES += ../../tmtests/include/timesys.h
psxtmcond08_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond08_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -DUSE_WAIT

LINK_OBJS = $(psxtmcond08_OBJECTS) $(psxtmcond08_LDADD)
LINK_LIBS = $(psxtmcond08_LDLIBS)

psxtmcond08$(EXEEXT): $(psxtmcond08_OBJECTS) $(psxtmcond08_DEPENDENCIES)
	@rm -f psxtmcond08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
