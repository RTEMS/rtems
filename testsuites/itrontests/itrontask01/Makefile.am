##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = itrontask01.exe
itrontask01_exe_SOURCES = init.c task1.c system.h

scndir = $(rtems_testsdir)
dist_scn_DATA = itrontask01.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = itrontask01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

itrontask01_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(itrontask01_exe_OBJECTS) $(itrontask01_exe_LDADD)
LINK_LIBS = $(itrontask01_exe_LDLIBS)

itrontask01.exe$(EXEEXT): $(itrontask01_exe_OBJECTS) $(itrontask01_exe_DEPENDENCIES)
	@rm -f itrontask01.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
