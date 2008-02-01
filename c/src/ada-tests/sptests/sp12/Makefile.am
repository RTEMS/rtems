## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp12.exe
ada_sp12_exe_SOURCES = sp12.adb config.h sptest.adb sptest.ads
ada_sp12_exe_SOURCES += ../../support/init.c

ada_sp12.exe: sp12.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp12.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
