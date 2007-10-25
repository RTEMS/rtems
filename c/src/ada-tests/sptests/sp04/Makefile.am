## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp04.exe
sp04_exe_SOURCES = sp04.adb config.h sptest.adb sptest.ads
sp04_exe_SOURCES += ../../support/init.c

sp04.exe: sp04.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp04.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
