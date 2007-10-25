## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp15.exe
sp15_exe_SOURCES = sp15.adb config.h sptest.adb sptest.ads
sp15_exe_SOURCES += ../../support/init.c

sp15.exe: sp15.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp15.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
