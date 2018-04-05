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
librtemsbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    ../../shared/bspstart.c startup/init5206e.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
# console
librtemsbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# i2c
librtemsbsp_a_SOURCES += i2c/i2c.c i2c/i2cdrv.c
# tod
librtemsbsp_a_SOURCES += tod/ds1307.c tod/todcfg.c ../../shared/tod.c
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
