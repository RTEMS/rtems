include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp14_ada_mp14_node2
mp14_ada_mp14_node2_SOURCES = mp14_node2.adb ../mptest.adb config.h
mp14_ada_mp14_node2_SOURCES += ../mptest.ads
mp14_ada_mp14_node2_SOURCES += ../../../support/init.c

mp14_ada_mp14_node2$(EXEEXT): mp14_node2.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_mp14-node2.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
