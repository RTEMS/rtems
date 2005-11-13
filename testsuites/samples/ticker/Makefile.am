##
##  $Id$
##

MANAGERS = io

rtems_tests_PROGRAMS = ticker.exe
ticker_exe_SOURCES = init.c tasks.c system.h

scndir = $(rtems_testsdir)
dist_scn_DATA = ticker.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = ticker.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

ticker_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

LINK_OBJS = $(ticker_exe_OBJECTS) $(ticker_exe_LDADD)
LINK_LIBS = $(ticker_exe_LDLIBS)

ticker.exe$(EXEEXT): $(ticker_exe_OBJECTS) $(ticker_exe_DEPENDENCIES)
	@rm -f ticker.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
