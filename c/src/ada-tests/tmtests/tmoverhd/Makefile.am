## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = tmoverhd
tmoverhd_SOURCES = tmoverhd.adb config.h \
tmtest.adb tmtest.ads \
dummy_rtems.adb dummy_rtems.ads
tmoverhd_SOURCES += ../../support/init.c

tmoverhd$(EXEEXT): tmoverhd.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../tests/automake/local.am
