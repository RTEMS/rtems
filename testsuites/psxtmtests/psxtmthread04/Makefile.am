MANAGERS = all

rtems_tests_PROGRAMS = psxtmthread04
psxtmthread04_SOURCES  = init.c
psxtmthread04_SOURCES += ../../tmtests/include/timesys.h
psxtmthread04_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmthread04_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmthread04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmthread04_OBJECTS) $(psxtmthread04_LDADD)
LINK_LIBS = $(psxtmthread04_LDLIBS)

psxtmthread04$(EXEEXT): $(psxtmthread04_OBJECTS) $(psxtmthread04_DEPENDENCIES)
	@rm -f psxtmthread04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
