## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp17.exe
sp17_exe_SOURCES = sp17.adb config.h sptest.adb sptest.ads
sp17_exe_SOURCES += ../../support/init.c

sp17.exe: sp17.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp17.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
