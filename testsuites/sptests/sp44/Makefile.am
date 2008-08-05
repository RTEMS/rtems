##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp44.exe
sp44_exe_SOURCES = init.c

dist_rtems_tests_DATA = sp44.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp44_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp44_exe_OBJECTS) $(sp44_exe_LDADD)
LINK_LIBS = $(sp44_exe_LDLIBS)

sp44.exe$(EXEEXT): $(sp44_exe_OBJECTS) $(sp44_exe_DEPENDENCIES)
	@rm -f sp44.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
