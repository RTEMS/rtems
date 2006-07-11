##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itronsem01.exe
itronsem01_exe_SOURCES = init.c system.h

dist_rtems_tests_DATA = itronsem01.scn
dist_rtems_tests_DATA += itronsem01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itronsem01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itronsem01_exe_OBJECTS) $(itronsem01_exe_LDADD)
LINK_LIBS = $(itronsem01_exe_LDLIBS)

itronsem01.exe$(EXEEXT): $(itronsem01_exe_OBJECTS) $(itronsem01_exe_DEPENDENCIES)
	@rm -f itronsem01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
