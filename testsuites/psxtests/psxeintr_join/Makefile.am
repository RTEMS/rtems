##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxeintr_join.exe
psxeintr_join_exe_SOURCES = init.c

dist_rtems_tests_DATA = psxeintr_join.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxeintr_join_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxeintr_join_exe_OBJECTS) $(psxeintr_join_exe_LDADD)
LINK_LIBS = $(psxeintr_join_exe_LDLIBS)

psxeintr_join.exe$(EXEEXT): $(psxeintr_join_exe_OBJECTS) $(psxeintr_join_exe_DEPENDENCIES)
	@rm -f psxeintr_join.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
