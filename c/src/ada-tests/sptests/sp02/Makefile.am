## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = sp02.exe
sp02_exe_SOURCES = sp02.adb config.h sptest.adb sptest.ads
sp02_exe_SOURCES += ../../support/init.c

sp02.exe: sp02.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = sp02.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
