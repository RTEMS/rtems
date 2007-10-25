## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tmoverhd.exe
tmoverhd_exe_SOURCES = tmoverhd.adb config.h tmtest.adb tmtest.ads \
    dummy_rtems.adb dummy_rtems.ads
tmoverhd_exe_SOURCES += ../../support/init.c

tmoverhd.exe: tmoverhd.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
