include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm06
ada_tm06_SOURCES = tm06.adb config.h tmtest.adb tmtest.ads
ada_tm06_SOURCES += ../../support/init.c

ada_tm06$(EXEEXT): tm06.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
