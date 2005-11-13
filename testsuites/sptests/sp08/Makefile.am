##
## $Id$
##

MANAGERS = io

rtems_tests_PROGRAMS = sp08.exe
sp08_exe_SOURCES = init.c task1.c system.h

scndir = $(rtems_testsdir)
dist_scn_DATA = sp08.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = sp08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp08_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp08_exe_OBJECTS) $(sp08_exe_LDADD)
LINK_LIBS = $(sp08_exe_LDLIBS)

sp08.exe$(EXEEXT): $(sp08_exe_OBJECTS) $(sp08_exe_DEPENDENCIES)
	@rm -f sp08.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
