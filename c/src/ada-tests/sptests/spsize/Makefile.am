## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

if EXPADA
noinst_PROGRAMS = spsize
endif

spsize_SOURCES = spsize.adb sptest.adb sptest.ads

spsize$(EXEEXT): spsize.adb
	$(GNATCOMPILE) -margs -a $< -o $@

include $(top_srcdir)/../../../testsuites/automake/local.am
