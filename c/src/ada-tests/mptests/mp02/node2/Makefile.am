include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

noinst_PROGRAMS = mp02_ada_mp02_node2
mp02_ada_mp02_node2_SOURCES = mp02_node2.adb ../mptest.adb config.h
mp02_ada_mp02_node2_SOURCES += ../mptest.ads
mp02_ada_mp02_node2_SOURCES += ../../../support/init.c

mp02_ada_mp02_node2$(EXEEXT): mp02_node2.adb ../mptest.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_mp02-node2.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
