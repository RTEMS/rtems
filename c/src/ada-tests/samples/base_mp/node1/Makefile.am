## $Id$

NODE=1

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

include $(top_srcdir)/ada.am

AM_ADAFLAGS += -I$(srcdir)/..

node1$(EXEEXT): ../mptest.adb init.o
	$(GNATCOMPILE) -margs -a $< -o $@

init.o: ../../../support/init.c
	$(COMPILE.c) -DNODE_NUMBER=$(NODE) -I$(srcdir)/.. -c $<

if EXPADA
noinst_PROGRAMS = node1
endif

EXTRA_DIST = base_mp.scn

include $(top_srcdir)/../../../automake/local.am
