
rtems_tests_PROGRAMS = psxtmrwlock06
psxtmrwlock06_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmrwlock06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmrwlock06_OBJECTS)
LINK_LIBS = $(psxtmrwlock06_LDLIBS)

psxtmrwlock06$(EXEEXT): $(psxtmrwlock06_OBJECTS) $(psxtmrwlock06_DEPENDENCIES)
	@rm -f psxtmrwlock06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
