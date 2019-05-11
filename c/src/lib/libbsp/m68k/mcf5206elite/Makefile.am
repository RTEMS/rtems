ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/mcf5206elite/start/bsp_specs

start.$(OBJEXT): ../../../../../../bsps/m68k/mcf5206elite/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/m68k/mcf5206elite/start/linkcmds.flash

dist_project_lib_DATA += ../../../../../../bsps/m68k/mcf5206elite/start/gdbinit

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/start/init5206e.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/printk-dummy.c
# i2c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/i2c/i2c.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/i2c/i2cdrv.c
# tod
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/rtc/ds1307.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/rtc/todcfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
# nvram
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/nvram/nvram.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/ckinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/mcfmbus.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/mcfuart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/timerisr.S

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5206elite/headers.am
