MANAGERS = all

rtems_tests_PROGRAMS = psxtmcond04
psxtmcond04_SOURCES  = init.c
psxtmcond04_SOURCES += ../../tmtests/include/timesys.h
psxtmcond04_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmcond04_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmcond04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmcond04_OBJECTS) $(psxtmcond04_LDADD)
LINK_LIBS = $(psxtmcond04_LDLIBS)

psxtmcond04$(EXEEXT): $(psxtmcond04_OBJECTS) $(psxtmcond04_DEPENDENCIES)
	@rm -f psxtmcond04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
