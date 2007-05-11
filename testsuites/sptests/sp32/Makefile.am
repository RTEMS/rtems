##
## $Id$
##

MANAGERS = io rate_monotonic semaphore clock

rtems_tests_PROGRAMS = sp32.exe
sp32_exe_SOURCES = init.c

dist_rtems_tests_DATA = sp32.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp32_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp32_exe_OBJECTS) $(sp32_exe_LDADD)
LINK_LIBS = $(sp32_exe_LDLIBS)

sp32.exe$(EXEEXT): $(sp32_exe_OBJECTS) $(sp32_exe_DEPENDENCIES)
	@rm -f sp32.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
