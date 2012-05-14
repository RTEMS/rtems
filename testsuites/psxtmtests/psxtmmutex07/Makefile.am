
rtems_tests_PROGRAMS = psxtmmutex07
psxtmmutex07_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmmutex07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmmutex07_OBJECTS)
LINK_LIBS = $(psxtmmutex07_LDLIBS)

psxtmmutex07$(EXEEXT): $(psxtmmutex07_OBJECTS) $(psxtmmutex07_DEPENDENCIES)
	@rm -f psxtmmutex07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
