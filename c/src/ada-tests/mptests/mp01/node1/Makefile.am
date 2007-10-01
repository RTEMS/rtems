## $Id$

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

node1$(EXEEXT): ../mptest.adb
	$(GNATCOMPILE) -margs -a $< -o $@

if EXPADA
noinst_PROGRAMS = node1
endif

scndir = $(rtems_ada_testsdir)
dist_scn_DATA = mp01-node1.scn

include $(top_srcdir)/../../../testsuites/automake/local.am
