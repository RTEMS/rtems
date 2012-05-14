
rtems_tests_PROGRAMS = psxtmmutex03
psxtmmutex03_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmmutex03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmmutex03_OBJECTS)
LINK_LIBS = $(psxtmmutex03_LDLIBS)

psxtmmutex03$(EXEEXT): $(psxtmmutex03_OBJECTS) $(psxtmmutex03_DEPENDENCIES)
	@rm -f psxtmmutex03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
