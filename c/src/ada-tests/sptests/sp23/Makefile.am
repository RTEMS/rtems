## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp23.exe
sp23_exe_SOURCES = sp23.adb config.h sptest.adb sptest.ads
sp23_exe_SOURCES += ../../support/init.c

sp23.exe: sp23.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp23.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
