##
## $Id$
##

MANAGERS = io rate_monotonic semaphore

rtems_tests_PROGRAMS = rtmonuse.exe
rtmonuse_exe_SOURCES = init.c getall.c task1.c system.h

dist_rtems_tests_DATA = rtmonuse.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

rtmonuse_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(rtmonuse_exe_OBJECTS) $(rtmonuse_exe_LDADD)
LINK_LIBS = $(rtmonuse_exe_LDLIBS)

rtmonuse.exe$(EXEEXT): $(rtmonuse_exe_OBJECTS) $(rtmonuse_exe_DEPENDENCIES)
	@rm -f rtmonuse.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
