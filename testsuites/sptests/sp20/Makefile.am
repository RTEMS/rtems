##
## $Id$
##

MANAGERS = io rate_monotonic semaphore clock

rtems_tests_PROGRAMS = sp20.exe
sp20_exe_SOURCES = init.c getall.c task1.c system.h

dist_rtems_tests_DATA = sp20.scn
dist_rtems_tests_DATA += sp20.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp20_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp20_exe_OBJECTS) $(sp20_exe_LDADD)
LINK_LIBS = $(sp20_exe_LDLIBS)

sp20.exe$(EXEEXT): $(sp20_exe_OBJECTS) $(sp20_exe_DEPENDENCIES)
	@rm -f sp20.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
