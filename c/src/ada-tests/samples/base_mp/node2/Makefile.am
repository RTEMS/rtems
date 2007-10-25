## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

node2.exe: ../mptest.adb init.o
	$(GNATCOMPILE) -margs -a $< -o $@

init.o: ../../../support/init.c
	$(COMPILE.c) -DNODE_NUMBER=2 -I$(srcdir)/.. -c $<

if EXPADA
noinst_PROGRAMS = node2.exe
endif

node2_exe_SOURCES = mptest-per_node_configuration.adb

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = base_mp-node2.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
