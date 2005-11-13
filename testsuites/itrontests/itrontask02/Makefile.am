##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itrontask02.exe
itrontask02_exe_SOURCES = init.c dormant.c system.h

scndir = $(rtems_testsdir)
dist_scn_DATA = itrontask02.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = itrontask02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itrontask02_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itrontask02_exe_OBJECTS) $(itrontask02_exe_LDADD)
LINK_LIBS = $(itrontask02_exe_LDLIBS)

itrontask02.exe$(EXEEXT): $(itrontask02_exe_OBJECTS) $(itrontask02_exe_DEPENDENCIES)
	@rm -f itrontask02.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
