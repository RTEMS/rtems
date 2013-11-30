MANAGERS = all

rtems_tests_PROGRAMS = psxtmbarrier04
psxtmbarrier04_SOURCES  = init.c
psxtmbarrier04_SOURCES += ../../tmtests/include/timesys.h
psxtmbarrier04_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmbarrier04_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmbarrier04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmbarrier04_OBJECTS) $(psxtmbarrier04_LDADD)
LINK_LIBS = $(psxtmbarrier04_LDLIBS)

psxtmbarrier04$(EXEEXT): $(psxtmbarrier04_OBJECTS) $(psxtmbarrier04_DEPENDENCIES)
	@rm -f psxtmbarrier04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
