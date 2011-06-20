##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxautoinit01
psxautoinit01_SOURCES = init.c ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_rtems_tests_DATA = psxautoinit01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxautoinit01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxautoinit01_OBJECTS) $(psxautoinit01_LDADD)
LINK_LIBS = $(psxautoinit01_LDLIBS)

psxautoinit01$(EXEEXT): $(psxautoinit01_OBJECTS) \
    $(psxautoinit01_DEPENDENCIES)
	@rm -f psxautoinit01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
