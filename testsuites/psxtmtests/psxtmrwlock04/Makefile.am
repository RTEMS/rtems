
rtems_tests_PROGRAMS = psxtmrwlock04
psxtmrwlock04_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmrwlock04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmrwlock04_OBJECTS)
LINK_LIBS = $(psxtmrwlock04_LDLIBS)

psxtmrwlock04$(EXEEXT): $(psxtmrwlock04_OBJECTS) $(psxtmrwlock04_DEPENDENCIES)
	@rm -f psxtmrwlock04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
