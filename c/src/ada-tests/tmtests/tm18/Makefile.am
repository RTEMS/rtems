## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm18.exe
ada_tm18_exe_SOURCES = tm18.adb config.h tmtest.adb tmtest.ads
ada_tm18_exe_SOURCES += ../../support/init.c

ada_tm18.exe: tm18.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
