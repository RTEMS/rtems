
rtems_tests_PROGRAMS = psxtmbarrier03
psxtmbarrier03_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmbarrier03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmbarrier03_OBJECTS)
LINK_LIBS = $(psxtmbarrier03_LDLIBS)

psxtmbarrier03$(EXEEXT): $(psxtmbarrier03_OBJECTS) $(psxtmbarrier03_DEPENDENCIES)
	@rm -f psxtmbarrier03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
