## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

base_sp$(EXEEXT): base_sp.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

init.$(OBJEXT): ../../support/init.c
	$(COMPILE.c) -I$(srcdir) -c $<

noinst_PROGRAMS = base_sp

base_sp_SOURCES = base_sp.adb \
config.h sptest.adb sptest.ads

EXTRA_DIST = base_sp.scn

include $(top_srcdir)/../../../automake/local.am
