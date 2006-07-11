##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxfile01.exe
psxfile01_exe_SOURCES = main.c test.c test_cat.c test_extend.c test_write.c ../include/pmacros.h

dist_rtems_tests_DATA = psxfile01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxfile01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxfile01_exe_OBJECTS) $(psxfile01_exe_LDADD)
LINK_LIBS = $(psxfile01_exe_LDLIBS)

psxfile01.exe$(EXEEXT): $(psxfile01_exe_OBJECTS) $(psxfile01_exe_DEPENDENCIES)
	@rm -f psxfile01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
