MANAGERS = all

rtems_tests_PROGRAMS = deadlockbrk
deadlockbrk_SOURCES  = deadlockbrk.c
deadlockbrk_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = deadlockbrk.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(deadlockbrk_OBJECTS) $(deadlockbrk_LDADD)
LINK_LIBS = $(deadlockbrk_LDLIBS)

deadlockbrk$(EXEEXT): $(deadlockbrk_OBJECTS) $(deadlockbrk_DEPENDENCIES)
	@rm -f deadlockbrk$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
