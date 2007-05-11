##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spsize.exe
spsize_exe_SOURCES = init.c getint.c size.c system.h

dist_rtems_tests_DATA = spsize.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spsize_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsize_exe_OBJECTS) $(spsize_exe_LDADD)
LINK_LIBS = $(spsize_exe_LDLIBS)

spsize.exe$(EXEEXT): $(spsize_exe_OBJECTS) $(spsize_exe_DEPENDENCIES)
	@rm -f spsize.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
