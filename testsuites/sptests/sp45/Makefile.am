##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp45.exe
sp45_exe_SOURCES = init.c

dist_rtems_tests_DATA = sp45.scn
dist_rtems_tests_DATA += sp45.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp45_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp45_exe_OBJECTS) $(sp45_exe_LDADD)
LINK_LIBS = $(sp45_exe_LDLIBS)

sp45.exe$(EXEEXT): $(sp45_exe_OBJECTS) $(sp45_exe_DEPENDENCIES)
	@rm -f sp45.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
