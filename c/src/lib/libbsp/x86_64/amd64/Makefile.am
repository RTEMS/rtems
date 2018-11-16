ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/x86_64/amd64/start/bsp_specs

noinst_PROGRAMS =

start.$(OBJEXT): ../../../../../../bsps/x86_64/amd64/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
#  XXX: We may want a custom bsp_work_area_initialize to detect memory size like
#  the i386
#
# FreeBSD's bootloader may leave a bootinfo structure for the kernel to find later:
# http://fxr.watson.org/fxr/source/i386/include/bootinfo.h?v=FREEBSD11#L48
#
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/start/page.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# interrupts
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/interrupts/pic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/interrupts/idt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/interrupts/isr_handler.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
# clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/clock/clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/console/console.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c
# cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/x86_64/amd64/headers.am
