include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm12
ada_tm12_SOURCES = tm12.adb config.h tmtest.adb tmtest.ads
ada_tm12_SOURCES += ../../support/init.c

ada_tm12$(EXEEXT): tm12.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
