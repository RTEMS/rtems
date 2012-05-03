include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp13_ada_mp13_node1
mp13_ada_mp13_node1_SOURCES = mp13_node1.adb ../mptest.adb config.h
mp13_ada_mp13_node1_SOURCES += ../mptest.ads
mp13_ada_mp13_node1_SOURCES += ../../../support/init.c

mp13_ada_mp13_node1$(EXEEXT): mp13_node1.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_mp13-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
