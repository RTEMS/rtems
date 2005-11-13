##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = sp29.exe
sp29_exe_SOURCES = init.c

scndir = $(rtems_testsdir)
dist_scn_DATA = sp29.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = sp29.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

sp29_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp29_exe_OBJECTS) $(sp29_exe_LDADD)
LINK_LIBS = $(sp29_exe_LDLIBS)

sp29.exe$(EXEEXT): $(sp29_exe_OBJECTS) $(sp29_exe_DEPENDENCIES)
	@rm -f sp29.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
