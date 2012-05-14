
rtems_tests_PROGRAMS = psxtmkey01
psxtmkey01_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmkey01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmkey01_OBJECTS)
LINK_LIBS = $(psxtmkey01_LDLIBS)

psxtmkey01$(EXEEXT): $(psxtmkey01_OBJECTS) $(psxtmkey01_DEPENDENCIES)
	@rm -f psxtmkey01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
