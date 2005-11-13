##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itronmbf01.exe
itronmbf01_exe_SOURCES = init.c system.h

scndir = $(rtems_testsdir)
dist_scn_DATA = itronmbf01.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = itronmbf01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itronmbf01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itronmbf01_exe_OBJECTS) $(itronmbf01_exe_LDADD)
LINK_LIBS = $(itronmbf01_exe_LDLIBS)

itronmbf01.exe$(EXEEXT): $(itronmbf01_exe_OBJECTS) $(itronmbf01_exe_DEPENDENCIES)
	@rm -f itronmbf01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
