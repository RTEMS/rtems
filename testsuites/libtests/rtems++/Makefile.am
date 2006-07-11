##
## $Id$
##

MANAGERS = event io msg sem signal timer rate_monotonic

if HAS_CXX
rtems_tests_PROGRAMS = rtems++.exe
rtems___exe_SOURCES = Init.cc Task1.cc Task2.cc Task3.cc System.h
endif

dist_rtems_tests_DATA = rtems++.scn
dist_rtems_tests_DATA += rtems++.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if HAS_CXX
rtems___exe_LDLIBS = -lrtems++
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

rtems___exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(rtems___exe_OBJECTS) $(rtems___exe_LDADD)
LINK_LIBS = $(rtems___exe_LDLIBS)

rtems++.exe$(EXEEXT): $(rtems___exe_OBJECTS) $(rtems___exe_DEPENDENCIES)
	@rm -f rtems++.exe$(EXEEXT)
	$(make-cxx-exe)
endif

include $(top_srcdir)/../automake/local.am
