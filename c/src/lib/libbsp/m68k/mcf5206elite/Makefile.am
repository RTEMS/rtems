ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


_SUBDIRS = . tools



start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.flash

dist_project_lib_DATA += startup/gdbinit

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += startup/init5206e.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# console
librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += ../../shared/dummy_printk_support.c
# i2c
librtemsbsp_a_SOURCES += i2c/i2c.c
librtemsbsp_a_SOURCES += i2c/i2cdrv.c
# tod
librtemsbsp_a_SOURCES += tod/ds1307.c
librtemsbsp_a_SOURCES += tod/todcfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
# nvram
librtemsbsp_a_SOURCES += nvram/nvram.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/ckinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/mcfmbus.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/mcfuart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/timerisr.S

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5206elite/headers.am
