MANAGERS = all

rtems_tests_PROGRAMS = psxtmthread06
psxtmthread06_SOURCES  = init.c
psxtmthread06_SOURCES += ../../tmtests/include/timesys.h
psxtmthread06_SOURCES += ../../support/src/tmtests_empty_function.c
psxtmthread06_SOURCES += ../../support/src/tmtests_support.c

dist_rtems_tests_DATA = psxtmthread06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxtmthread06_OBJECTS) $(psxtmthread06_LDADD)
LINK_LIBS = $(psxtmthread06_LDLIBS)

psxtmthread06$(EXEEXT): $(psxtmthread06_OBJECTS) $(psxtmthread06_DEPENDENCIES)
	@rm -f psxtmthread06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
