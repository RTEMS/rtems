## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

node1.exe: ../mptest.adb init.o
	$(GNATCOMPILE) -margs -a $< -o $@

init.o: ../../../support/init.c
	$(COMPILE.c) -DNODE_NUMBER=1 -I$(srcdir)/.. -c $<

if EXPADA
noinst_PROGRAMS = node1.exe
endif

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = base_mp-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
