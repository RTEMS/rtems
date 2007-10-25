## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm01.exe
tm01_exe_SOURCES = tm01.adb config.h tmtest.adb tmtest.ads
tm01_exe_SOURCES += ../../support/init.c

tm01.exe: tm01.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
