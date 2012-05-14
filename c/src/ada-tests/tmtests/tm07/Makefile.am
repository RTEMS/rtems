include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm07
ada_tm07_SOURCES = tm07.adb config.h tmtest.adb tmtest.ads
ada_tm07_SOURCES += ../../support/init.c

ada_tm07$(EXEEXT): tm07.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
