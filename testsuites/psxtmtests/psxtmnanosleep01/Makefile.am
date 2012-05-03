
rtems_tests_PROGRAMS = psxtmnanosleep01
psxtmnanosleep01_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmnanosleep01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmnanosleep01_OBJECTS)
LINK_LIBS = $(psxtmnanosleep01_LDLIBS)

psxtmnanosleep01$(EXEEXT): $(psxtmnanosleep01_OBJECTS) $(psxtmnanosleep01_DEPENDENCIES)
	@rm -f psxtmnanosleep01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
