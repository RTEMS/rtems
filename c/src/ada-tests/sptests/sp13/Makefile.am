## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp13.exe
sp13_exe_SOURCES = sp13.adb config.h sptest.adb sptest.ads
sp13_exe_SOURCES += ../../support/init.c

sp13.exe: sp13.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp13.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
