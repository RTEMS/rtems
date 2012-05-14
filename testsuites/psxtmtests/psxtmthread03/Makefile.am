
rtems_tests_PROGRAMS = psxtmthread03
psxtmthread03_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmthread03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmthread03_OBJECTS)
LINK_LIBS = $(psxtmthread03_LDLIBS)

psxtmthread03$(EXEEXT): $(psxtmthread03_OBJECTS) $(psxtmthread03_DEPENDENCIES)
	@rm -f psxtmthread03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
