## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp01.exe
sp01_exe_SOURCES = sp01.adb config.h sptest.adb sptest.ads
sp01_exe_SOURCES += ../../support/init.c

sp01.exe: sp01.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp01.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
