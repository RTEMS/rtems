## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp07.exe

sp07_exe_SOURCES = sp07.adb config.h sptest.adb sptest.ads
sp07_exe_SOURCES += ../../support/init.c

sp07.exe: sp07.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp07.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
