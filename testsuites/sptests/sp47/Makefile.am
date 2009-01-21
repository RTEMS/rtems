##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp47.exe
sp47_exe_SOURCES = init.c

dist_rtems_tests_DATA = sp47.scn
dist_rtems_tests_DATA += sp47.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp47_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp47_exe_OBJECTS) $(sp47_exe_LDADD)
LINK_LIBS = $(sp47_exe_LDLIBS)

sp47.exe$(EXEEXT): $(sp47_exe_OBJECTS) $(sp47_exe_DEPENDENCIES)
	@rm -f sp47.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
