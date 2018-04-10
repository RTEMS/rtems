ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

startup_SOURCES = ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspreset.c \
    startup/bspstart.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c startup/crtn.S
clock_SOURCES = ../../shared/clock_driver_simidle.c
console_SOURCES = ../../shared/console-polled.c console/console-io.c \
   console/syscalls.S
timer_SOURCES = timer/timer.c

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES = $(startup_SOURCES)
librtemsbsp_a_SOURCES += $(clock_SOURCES)
librtemsbsp_a_SOURCES += $(console_SOURCES)
librtemsbsp_a_SOURCES += $(timer_SOURCES)

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m32c/m32cbsp/headers.am
