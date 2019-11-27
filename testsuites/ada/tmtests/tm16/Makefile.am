include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_tm16
ada_tm16_SOURCES = tm16.adb config.h tmtest.adb tmtest.ads
ada_tm16_SOURCES += init.c

ada_tm16$(EXEEXT): tm16.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

