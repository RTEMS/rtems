
rtems_tests_PROGRAMS = psxtmsem02
psxtmsem02_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmsem02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmsem02_OBJECTS)
LINK_LIBS = $(psxtmsem02_LDLIBS)

psxtmsem02$(EXEEXT): $(psxtmsem02_OBJECTS) $(psxtmsem02_DEPENDENCIES)
	@rm -f psxtmsem02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
