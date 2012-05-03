
rtems_tests_PROGRAMS = psxtmrwlock01
psxtmrwlock01_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmrwlock01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmrwlock01_OBJECTS)
LINK_LIBS = $(psxtmrwlock01_LDLIBS)

psxtmrwlock01$(EXEEXT): $(psxtmrwlock01_OBJECTS) $(psxtmrwlock01_DEPENDENCIES)
	@rm -f psxtmrwlock01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
