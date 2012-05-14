include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp03_ada_mp03_node1
mp03_ada_mp03_node1_SOURCES = mp03_node1.adb ../mptest.adb config.h
mp03_ada_mp03_node1_SOURCES += ../mptest.ads
mp03_ada_mp03_node1_SOURCES += ../../../support/init.c

mp03_ada_mp03_node1$(EXEEXT): mp03_node1.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_mp03-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
