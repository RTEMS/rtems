include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp15
ada_sp15_SOURCES = sp15.adb config.h sptest.adb sptest.ads
ada_sp15_SOURCES += ../../support/init.c

ada_sp15$(EXEEXT): sp15.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp15.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
