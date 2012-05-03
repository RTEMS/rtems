include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp04_ada_mp04_node1
mp04_ada_mp04_node1_SOURCES = mp04_node1.adb ../mptest.adb config.h
mp04_ada_mp04_node1_SOURCES += ../mptest.ads
mp04_ada_mp04_node1_SOURCES += ../../../support/init.c

mp04_ada_mp04_node1$(EXEEXT): mp04_node1.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_mp04-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
