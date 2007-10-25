## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp16.exe
sp16_exe_SOURCES = sp16.adb config.h sptest.adb sptest.ads
sp16_exe_SOURCES += ../../support/init.c

sp16.exe: sp16.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp16.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
