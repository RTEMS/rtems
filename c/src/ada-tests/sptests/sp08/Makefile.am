## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp08.exe
sp08_exe_SOURCES = sp08.adb config.h sptest.adb sptest.ads
sp08_exe_SOURCES += ../../support/init.c

sp08.exe: sp08.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp08.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
