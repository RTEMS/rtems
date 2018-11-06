MANAGERS = all

rtems_tests_PROGRAMS = psxtmonce01
psxtmonce01_SOURCES  = init.c
psxtmonce01_SOURCES += ../../tmtests/include/timesys.h
psxtmonce01_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmonce01_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmonce01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmonce01_OBJECTS) $(psxtmonce01_LDADD)
LINK_LIBS = $(psxtmonce01_LDLIBS)

psxtmonce01$(EXEEXT): $(psxtmonce01_OBJECTS) $(psxtmonce01_DEPENDENCIES)
	@rm -f psxtmonce01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
