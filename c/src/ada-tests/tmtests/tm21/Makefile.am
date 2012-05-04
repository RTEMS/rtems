include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm21
ada_tm21_SOURCES = tm21.adb config.h tmtest.adb tmtest.ads
ada_tm21_SOURCES += ../../support/init.c

ada_tm21$(EXEEXT): tm21.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
