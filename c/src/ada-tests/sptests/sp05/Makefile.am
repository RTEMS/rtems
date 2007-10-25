## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp05.exe
sp05_exe_SOURCES = sp05.adb config.h sptest.adb sptest.ads
sp05_exe_SOURCES += ../../support/init.c

sp05.exe: sp05.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp05.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
