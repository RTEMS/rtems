include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp02
ada_sp02_SOURCES = sp02.adb config.h sptest.adb sptest.ads
ada_sp02_SOURCES += init.c

ada_sp02$(EXEEXT): sp02.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp02.scn

