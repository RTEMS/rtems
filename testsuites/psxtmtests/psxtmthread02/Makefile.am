MANAGERS = all

rtems_tests_PROGRAMS = psxtmthread02
psxtmthread02_SOURCES  = init.c
psxtmthread02_SOURCES += ../../tmtests/include/timesys.h
psxtmthread02_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmthread02_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmthread02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmthread02_OBJECTS) $(psxtmthread02_LDADD)
LINK_LIBS = $(psxtmthread02_LDLIBS)

psxtmthread02$(EXEEXT): $(psxtmthread02_OBJECTS) $(psxtmthread02_DEPENDENCIES)
	@rm -f psxtmthread02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
