ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.t32mppc

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# Startup
librtemsbsp_a_SOURCES += \
	../../shared/bootcard.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bsppredriverhook.c \
	../../shared/sbrk.c \
	../../shared/gnatinstallhandler.c \
	../../shared/bspclean.c \
	../../shared/bspgetworkarea.c \
	../shared/src/ppc-exc-handler-table.c \
	../shared/src/tictac.c \
	../shared/src/bsp-start-zero.S \
	startup/bspstart.c \
	startup/bspreset.c

# Clock
librtemsbsp_a_SOURCES += clock/clock-config.c

# Timer
librtemsbsp_a_SOURCES += ../../shared/timerstub.c

# IRQ
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/console.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../shared/bspsmp.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/t32mppc/headers.am
