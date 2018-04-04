ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

if HAS_NETWORKING
endif

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

# Link commands
project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =
librtemsbsp_a_LIBADD =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bspgetworkarea.c \
    ../../shared/bspreset_loop.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../shared/src/bsp-start-zero.S

# bspconsole
librtemsbsp_a_SOURCES += console/consolelite.c ../../shared/console.c \
    ../../shared/console_select.c ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c 

# irq
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += irq/irq_init.c

# clock
librtemsbsp_a_SOURCES += ../shared/clock/clock.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += = network/xiltemac.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/virtex/headers.am
