
rtems_tests_PROGRAMS = psxtmsleep02
psxtmsleep02_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmsleep02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmsleep02_OBJECTS)
LINK_LIBS = $(psxtmsleep02_LDLIBS)

psxtmsleep02$(EXEEXT): $(psxtmsleep02_OBJECTS) $(psxtmsleep02_DEPENDENCIES)
	@rm -f psxtmsleep02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
