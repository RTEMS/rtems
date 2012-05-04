include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp17
ada_sp17_SOURCES = sp17.adb config.h sptest.adb sptest.ads
ada_sp17_SOURCES += ../../support/init.c

ada_sp17$(EXEEXT): sp17.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp17.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
