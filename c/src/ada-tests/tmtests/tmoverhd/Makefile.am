include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tmoverhd
ada_tmoverhd_SOURCES = tmoverhd.adb config.h tmtest.adb tmtest.ads \
    dummy_rtems.adb dummy_rtems.ads
ada_tmoverhd_SOURCES += ../../support/init.c

ada_tmoverhd$(EXEEXT): tmoverhd.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
