include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am

include $(top_srcdir)/ada.am

noinst_PROGRAMS = ada_sp23
ada_sp23_SOURCES = sp23.adb config.h sptest.adb sptest.ads
ada_sp23_SOURCES += init.c

ada_sp23$(EXEEXT): sp23.adb init.$(OBJEXT)
	$(GNATCOMPILE) -margs -a $< -o $@

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = ada_sp23.scn

