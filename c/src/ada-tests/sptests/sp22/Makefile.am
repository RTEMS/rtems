## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp22.exe
sp22_exe_SOURCES = sp22.adb config.h sptest.adb sptest.ads
sp22_exe_SOURCES += ../../support/init.c

sp22.exe: sp22.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp22.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
