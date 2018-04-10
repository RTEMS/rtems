ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =
librtemsbsp_a_LIBADD =

# startup
librtemsbsp_a_SOURCES += startup/bspclean.c
librtemsbsp_a_SOURCES += ../shared/startup/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c

# irq
librtemsbsp_a_SOURCES += ../shared/irq/irq.c

# clock
librtemsbsp_a_SOURCES += ../shared/milkymist_clock/ckinit.c

# console
librtemsbsp_a_SOURCES += ../shared/milkymist_console/console.c
librtemsbsp_a_SOURCES += ../shared/milkymist_console/uart.c

# timer
librtemsbsp_a_SOURCES += ../shared/milkymist_timer/timer.c

# framebuffer
librtemsbsp_a_SOURCES += ../shared/milkymist_framebuffer/framebuffer.c

# GPIO
librtemsbsp_a_SOURCES += ../shared/milkymist_gpio/gpio.c

# buttons
librtemsbsp_a_SOURCES += ../shared/milkymist_buttons/buttons.c

# ac97
librtemsbsp_a_SOURCES += ../shared/milkymist_ac97/ac97.c

# usbinput
librtemsbsp_a_SOURCES += ../shared/milkymist_usbinput/usbinput.c

# PFPU
librtemsbsp_a_SOURCES += ../shared/milkymist_pfpu/pfpu.c

# TMU
librtemsbsp_a_SOURCES += ../shared/milkymist_tmu/tmu.c

# memory card
librtemsbsp_a_SOURCES += ../shared/milkymist_memcard/memcard.c

# flash
librtemsbsp_a_SOURCES += ../shared/milkymist_flash/flash.c

# DMX
librtemsbsp_a_SOURCES += ../shared/milkymist_dmx/dmx.c

# MIDI
librtemsbsp_a_SOURCES += ../shared/milkymist_midi/midi.c

# IR
librtemsbsp_a_SOURCES += ../shared/milkymist_ir/ir.c

# video input
librtemsbsp_a_SOURCES += ../shared/milkymist_video/video.c

# versions
librtemsbsp_a_SOURCES += ../shared/milkymist_versions/versions.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../shared/milkymist_networking/network.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/lm32/milkymist/headers.am
