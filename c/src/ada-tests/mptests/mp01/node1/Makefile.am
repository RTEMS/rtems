include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp01_ada_mp01_node1
mp01_ada_mp01_node1_SOURCES = mp01_node1.adb ../mptest.adb config.h
mp01_ada_mp01_node1_SOURCES += ../mptest.ads
mp01_ada_mp01_node1_SOURCES += ../../../support/init.c

mp01_ada_mp01_node1$(EXEEXT): mp01_node1.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_mp01-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
