## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include $(top_srcdir)/ada.am

if EXPADA
noinst_PROGRAMS = tm26
endif
tm26_SOURCES = tm26.adb config.h tmtest.adp tmtest.ads

tm26$(EXEEXT): tm26.adp
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../automake/local.am
