##
## $Id$
##

MANAGERS = all

rtems_tests_PROGRAMS = spmkdir
spmkdir_SOURCES = init.c

dist_rtems_tests_DATA = spmkdir.scn
dist_rtems_tests_DATA += spmkdir.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

spmkdir_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spmkdir_OBJECTS) $(spmkdir_LDADD)
LINK_LIBS = $(spmkdir_LDLIBS)

spmkdir$(EXEEXT): $(spmkdir_OBJECTS) $(spmkdir_DEPENDENCIES)
	@rm -f spmkdir$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
