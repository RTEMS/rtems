
rtems_tests_PROGRAMS = psxtmmq01
psxtmmq01_SOURCES  = init.c
psxtmmq01_SOURCES += ../../tmtests/include/timesys.h
psxtmmq01_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmmq01_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmmq01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmmq01_OBJECTS)
LINK_LIBS = $(psxtmmq01_LDLIBS)

psxtmmq01$(EXEEXT): $(psxtmmq01_OBJECTS) $(psxtmmq01_DEPENDENCIES)
	@rm -f psxtmmq01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
