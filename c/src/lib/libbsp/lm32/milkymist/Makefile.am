ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS  = include/bsp.h
include_HEADERS += ../../shared/include/coverhd.h
include_HEADERS += ../../shared/include/tm27.h

include_bsp_HEADERS  = ../../lm32/shared/include/irq.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_gpio/milkymist_gpio.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_buttons/milkymist_buttons.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_ac97/milkymist_ac97.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_usbinput/milkymist_usbinput.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_pfpu/milkymist_pfpu.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_tmu/milkymist_tmu.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_memcard/milkymist_memcard.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_flash/milkymist_flash.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_dmx/milkymist_dmx.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_midi/milkymist_midi.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_ir/milkymist_ir.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_video/milkymist_video.h
include_bsp_HEADERS += ../../lm32/shared/milkymist_versions/milkymist_versions.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/system_conf.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = ../../lm32/shared/start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD =

# startup
libbsp_a_SOURCES += startup/bspclean.c
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../shared/startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bsppretaskinghook.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bootcard.c
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

if HAS_NETWORKING
noinst_PROGRAMS += network.rel
network_rel_SOURCES = ../../lm32/shared/milkymist_networking/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
