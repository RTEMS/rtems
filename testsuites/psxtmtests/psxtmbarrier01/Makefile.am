
rtems_tests_PROGRAMS = psxtmbarrier01
psxtmbarrier01_SOURCES = init.c ../../tmtests/include/timesys.h \
    ../../support/src/tmtests_empty_function.c \
    ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmbarrier01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmbarrier01_OBJECTS)
LINK_LIBS = $(psxtmbarrier01_LDLIBS)

psxtmbarrier01$(EXEEXT): $(psxtmbarrier01_OBJECTS) $(psxtmbarrier01_DEPENDENCIES)
	@rm -f psxtmbarrier01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
