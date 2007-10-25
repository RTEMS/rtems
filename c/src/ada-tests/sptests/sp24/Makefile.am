## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp24.exe
sp24_exe_SOURCES = sp24.adb config.h sptest.adb sptest.ads
sp24_exe_SOURCES += ../../support/init.c

sp24.exe: sp24.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp24.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
