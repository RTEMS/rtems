##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp28.exe
sp28_exe_SOURCES = init.c

dist_rtems_tests_DATA = sp28.scn
dist_rtems_tests_DATA += sp28.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp28_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp28_exe_OBJECTS) $(sp28_exe_LDADD)
LINK_LIBS = $(sp28_exe_LDLIBS)

sp28.exe$(EXEEXT): $(sp28_exe_OBJECTS) $(sp28_exe_DEPENDENCIES)
	@rm -f sp28.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
