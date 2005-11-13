##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itronmbox01.exe
itronmbox01_exe_SOURCES = init.c system.h

scndir = $(rtems_testsdir)
dist_scn_DATA = itronmbox01.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = itronmbox01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itronmbox01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itronmbox01_exe_OBJECTS) $(itronmbox01_exe_LDADD)
LINK_LIBS = $(itronmbox01_exe_LDLIBS)

itronmbox01.exe$(EXEEXT): $(itronmbox01_exe_OBJECTS) $(itronmbox01_exe_DEPENDENCIES)
	@rm -f itronmbox01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
