
rtems_tests_PROGRAMS = psxtmmutex01
psxtmmutex01_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmmutex01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmmutex01_OBJECTS)
LINK_LIBS = $(psxtmmutex01_LDLIBS)

psxtmmutex01$(EXEEXT): $(psxtmmutex01_OBJECTS) $(psxtmmutex01_DEPENDENCIES)
	@rm -f psxtmmutex01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
