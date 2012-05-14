
rtems_tests_PROGRAMS = psxtmsem05
psxtmsem05_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmsem05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmsem05_OBJECTS)
LINK_LIBS = $(psxtmsem05_LDLIBS)

psxtmsem05$(EXEEXT): $(psxtmsem05_OBJECTS) $(psxtmsem05_DEPENDENCIES)
	@rm -f psxtmsem05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
