##
## $Id$
##

MANAGERS = io mp semaphore

rtems_tests_PROGRAMS = mp08-node2.exe
mp08_node2_exe_SOURCES = ../init.c ../task1.c ../system.h

scndir = $(rtems_testsdir)
dist_scn_DATA = mp08-node2.scn
docdir = $(rtems_testsdir)
dist_doc_DATA = mp08-node2.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mp08_node2_exe_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel)

AM_CPPFLAGS += -DNODE_NUMBER=2 -I$(srcdir)/..

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mp08_node2_exe_OBJECTS) $(mp08_node2_exe_LDADD)
LINK_LIBS = $(mp08_node2_exe_LDLIBS)

mp08-node2.exe$(EXEEXT): $(mp08_node2_exe_OBJECTS) $(mp08_node2_exe_DEPENDENCIES)
	@rm -f mp08-node2.exe$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
