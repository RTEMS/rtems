
rtems_tests_PROGRAMS = psxtmclocknanosleep03
psxtmclocknanosleep03_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmclocknanosleep03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmclocknanosleep03_OBJECTS)
LINK_LIBS = $(psxtmclocknanosleep03_LDLIBS)

psxtmclocknanosleep03$(EXEEXT): $(psxtmclocknanosleep03_OBJECTS) $(psxtmclocknanosleep03_DEPENDENCIES)
	@rm -f psxtmclocknanosleep03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
