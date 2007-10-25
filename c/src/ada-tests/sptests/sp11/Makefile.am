## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp11.exe
sp11_exe_SOURCES = sp11.adb config.h sptest.adb sptest.ads
sp11_exe_SOURCES += ../../support/init.c

sp11.exe: sp11.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp11.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
