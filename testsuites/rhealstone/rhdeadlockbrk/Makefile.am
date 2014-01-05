MANAGERS = all

rtems_tests_PROGRAMS = rhdeadlockbrk
rhdeadlockbrk_SOURCES  = deadlockbrk.c
rhdeadlockbrk_SOURCES += ../../tmtests/include/timesys.h

dist_rtems_tests_DATA = rhdeadlockbrk.adoc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../tmtests/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rhdeadlockbrk_OBJECTS) $(rhdeadlockbrk_LDADD)
LINK_LIBS = $(rhdeadlockbrk_LDLIBS)

rhdeadlockbrk$(EXEEXT): $(rhdeadlockbrk_OBJECTS) $(rhdeadlockbrk_DEPENDENCIES)
	@rm -f rhdeadlockbrk$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
