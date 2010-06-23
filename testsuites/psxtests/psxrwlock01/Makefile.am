##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = psxrwlock01
psxrwlock01_SOURCES = main.c test.c ../include/pmacros.h

scndir = $(rtems_testsdir)
dist_scn_DATA = psxrwlock01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

psxrwlock01_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxrwlock01_OBJECTS) $(psxrwlock01_LDADD)
LINK_LIBS = $(psxrwlock01_LDLIBS)

psxrwlock01$(EXEEXT): $(psxrwlock01_OBJECTS) \
    $(psxrwlock01_DEPENDENCIES)
	@rm -f psxrwlock01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
