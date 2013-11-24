MANAGERS = all

rtems_tests_PROGRAMS = psxtmthread05
psxtmthread05_SOURCES  = init.c
psxtmthread05_SOURCES += ../../tmtests/include/timesys.h
psxtmthread05_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmthread05_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmthread05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmthread05_OBJECTS) $(psxtmthread05_LDADD)
LINK_LIBS = $(psxtmthread05_LDLIBS)

psxtmthread05$(EXEEXT): $(psxtmthread05_OBJECTS) $(psxtmthread05_DEPENDENCIES)
	@rm -f psxtmthread05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
