## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tm01
tm01_SOURCES = tm01.adb config.h tmtest.adb tmtest.ads
tm01_SOURCES += ../../support/init.c

tm01$(EXEEXT): tm01.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
