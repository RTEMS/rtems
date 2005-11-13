##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp27.exe
sp27_exe_SOURCES = init.c

scndir = $(rtems_testsdir)
dist_scn_DATA = sp27.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = sp27.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp27_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(sp27_exe_OBJECTS) $(sp27_exe_LDADD)
LINK_LIBS = $(sp27_exe_LDLIBS)

sp27.exe$(EXEEXT): $(sp27_exe_OBJECTS) $(sp27_exe_DEPENDENCIES)
	@rm -f sp27.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
