## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp09_node1.exe
mp09_node1_exe_SOURCES = mp09_node1.adb ../mptest.adb config.h ../mptest.ads
mp09_node1_exe_SOURCES += ../../../support/init.c

mp09_node1.exe: mp09_node1.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = mp09-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
