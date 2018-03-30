ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


start.$(OBJEXT): ../../lm32/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD =

# startup
libbsp_a_SOURCES += startup/bspclean.c
libbsp_a_SOURCES += ../shared/startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c

# irq
libbsp_a_SOURCES += ../../lm32/shared/irq/irq.c

# clock
libbsp_a_SOURCES += ../../lm32/shared/milkymist_clock/ckinit.c

# console
libbsp_a_SOURCES += ../../lm32/shared/milkymist_console/console.c
libbsp_a_SOURCES += ../../lm32/shared/milkymist_console/uart.c

# timer
libbsp_a_SOURCES += ../../lm32/shared/milkymist_timer/timer.c

# framebuffer
libbsp_a_SOURCES += ../../lm32/shared/milkymist_framebuffer/framebuffer.c

# GPIO
libbsp_a_SOURCES += ../../lm32/shared/milkymist_gpio/gpio.c

# buttons
libbsp_a_SOURCES += ../../lm32/shared/milkymist_buttons/buttons.c

# ac97
libbsp_a_SOURCES += ../../lm32/shared/milkymist_ac97/ac97.c

# usbinput
libbsp_a_SOURCES += ../../lm32/shared/milkymist_usbinput/usbinput.c

# PFPU
libbsp_a_SOURCES += ../../lm32/shared/milkymist_pfpu/pfpu.c

# TMU
libbsp_a_SOURCES += ../../lm32/shared/milkymist_tmu/tmu.c

# memory card
libbsp_a_SOURCES += ../../lm32/shared/milkymist_memcard/memcard.c

# flash
libbsp_a_SOURCES += ../../lm32/shared/milkymist_flash/flash.c

# DMX
libbsp_a_SOURCES += ../../lm32/shared/milkymist_dmx/dmx.c

# MIDI
libbsp_a_SOURCES += ../../lm32/shared/milkymist_midi/midi.c

# IR
libbsp_a_SOURCES += ../../lm32/shared/milkymist_ir/ir.c

# video input
libbsp_a_SOURCES += ../../lm32/shared/milkymist_video/video.c

# versions
libbsp_a_SOURCES += ../../lm32/shared/milkymist_versions/versions.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if HAS_NETWORKING
libbsp_a_SOURCES += ../../lm32/shared/milkymist_networking/network.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/lm32/milkymist/headers.am
