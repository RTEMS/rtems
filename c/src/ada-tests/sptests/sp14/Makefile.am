## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp14.exe
sp14_exe_SOURCES = sp14.adb config.h sptest.adb sptest.ads
sp14_exe_SOURCES += ../../support/init.c

sp14.exe: sp14.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp14.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
