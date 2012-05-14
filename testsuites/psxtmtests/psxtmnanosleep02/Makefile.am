
rtems_tests_PROGRAMS = psxtmnanosleep02
psxtmnanosleep02_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmnanosleep02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmnanosleep02_OBJECTS)
LINK_LIBS = $(psxtmnanosleep02_LDLIBS)

psxtmnanosleep02$(EXEEXT): $(psxtmnanosleep02_OBJECTS) $(psxtmnanosleep02_DEPENDENCIES)
	@rm -f psxtmnanosleep02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
