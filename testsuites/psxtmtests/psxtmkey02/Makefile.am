
rtems_tests_PROGRAMS = psxtmkey02
psxtmkey02_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmkey02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmkey02_OBJECTS)
LINK_LIBS = $(psxtmkey02_LDLIBS)

psxtmkey02$(EXEEXT): $(psxtmkey02_OBJECTS) $(psxtmkey02_DEPENDENCIES)
	@rm -f psxtmkey02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
