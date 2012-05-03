
rtems_tests_PROGRAMS = psxtmmutex05
psxtmmutex05_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmmutex05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmmutex05_OBJECTS)
LINK_LIBS = $(psxtmmutex05_LDLIBS)

psxtmmutex05$(EXEEXT): $(psxtmmutex05_OBJECTS) $(psxtmmutex05_DEPENDENCIES)
	@rm -f psxtmmutex05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
