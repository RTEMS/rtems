## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp16.exe
ada_sp16_exe_SOURCES = sp16.adb config.h sptest.adb sptest.ads
ada_sp16_exe_SOURCES += ../../support/init.c

ada_sp16.exe: sp16.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp16.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
