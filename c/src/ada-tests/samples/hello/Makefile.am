## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

hello$(EXEEXT): hello.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

init.$(OBJEXT): ../../support/init.c
	$(COMPILE.c) -I$(srcdir) -c $<

noinst_PROGRAMS = hello

hello_SOURCES = hello.adb \
config.h sptest.adb sptest.ads

EXTRA_DIST = hello.scn

include $(top_srcdir)/../../../automake/local.am
