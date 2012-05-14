include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm20
ada_tm20_SOURCES = tm20.adb config.h tmtest.adb tmtest.ads
ada_tm20_SOURCES += ../../support/init.c

ada_tm20$(EXEEXT): tm20.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
