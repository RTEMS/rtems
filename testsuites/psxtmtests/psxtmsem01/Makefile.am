
rtems_tests_PROGRAMS = psxtmsem01
psxtmsem01_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmsem01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmsem01_OBJECTS)
LINK_LIBS = $(psxtmsem01_LDLIBS)

psxtmsem01$(EXEEXT): $(psxtmsem01_OBJECTS) $(psxtmsem01_DEPENDENCIES)
	@rm -f psxtmsem01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
