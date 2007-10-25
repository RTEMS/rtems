## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp14_node1.exe
mp14_node1_exe_SOURCES = mp14_node1.adb ../mptest.adb config.h ../mptest.ads
mp14_node1_exe_SOURCES += ../../../support/init.c

mp14_node1.exe: mp14_node1.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = mp14-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
