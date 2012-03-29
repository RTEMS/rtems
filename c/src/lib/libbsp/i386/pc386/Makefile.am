##
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# FIXME: We must not include *.cfg
include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

CLEANFILES =

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

SUBDIRS = . tools

include_bsp_HEADERS  = ../../i386/shared/irq/irq.h
include_bsp_HEADERS += ../../i386/shared/irq/irq_asm.h
include_bsp_HEADERS += ../../i386/shared/comm/tty_drv.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += console/rtd316.h
include_bsp_HEADERS += console/exar17d15x.h

if HAS_SMP
include_bsp_HEADERS += ../../i386/shared/irq/apic.h
include_bsp_HEADERS += ../../i386/shared/smp/smp-imps.h
endif

include_HEADERS += include/crt.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST = start/start16.S
start16.$(OBJEXT): start/start16.S
	$(CPPASCOMPILE) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ -c $<

start16-elf32.$(OBJEXT): start16.$(OBJEXT)
	$(LD) -N -T $(top_srcdir)/startup/linkcmds -Ttext $(START16ADDR) -e start16 -nostdlib \
	  --oformat=elf32-i386 \
	-o $@ $<

start16.bin: start16-elf32.$(OBJEXT)
	$(OBJCOPY) -O binary $< $@
CLEANFILES += start16.bin

project_lib_DATA += start16.bin

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# clock
libbsp_a_SOURCES += clock/ckinit.c
libbsp_a_SOURCES += clock/todcfg.c
libbsp_a_SOURCES += ../../shared/tod.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = console/keyboard.h
include_rtems_HEADERS += console/kd.h
include_rtems_HEADERS += console/ps2_drv.h
include_rtems_HEADERS += ../../shared/console_private.h
include_rtems_HEADERS += console/vgacons.h

include_HEADERS += ../../i386/shared/comm/i386_io.h
# console
libbsp_a_SOURCES += console/inch.c
libbsp_a_SOURCES += console/outch.c
libbsp_a_SOURCES += console/defkeymap.c
libbsp_a_SOURCES += console/fb_vga.c
libbsp_a_SOURCES += console/keyboard.c
libbsp_a_SOURCES += console/pc_keyb.c
libbsp_a_SOURCES += console/ps2_mouse.c
libbsp_a_SOURCES += console/vgainit.c
libbsp_a_SOURCES += console/vt.c
libbsp_a_SOURCES += console/videoAsm.S
libbsp_a_SOURCES += console/kbd_parser.c
libbsp_a_SOURCES += console/serial_mouse_config.c
libbsp_a_SOURCES += ../../i386/shared/comm/uart.c
libbsp_a_SOURCES += ../../i386/shared/comm/tty_drv.c
libbsp_a_SOURCES += ../../shared/console.c
libbsp_a_SOURCES += console/console_select.c
libbsp_a_SOURCES += ../../shared/console_read.c
libbsp_a_SOURCES += ../../shared/console_write.c
libbsp_a_SOURCES += console/console_control.c
libbsp_a_SOURCES += console/conscfg.c
libbsp_a_SOURCES += console/printk_support.c
libbsp_a_SOURCES += console/vgacons.c
libbsp_a_SOURCES += console/exar17d15x.c
libbsp_a_SOURCES += console/rtd316.c

# gdb
libbsp_a_SOURCES += ../../i386/shared/comm/i386-stub.c
libbsp_a_SOURCES += ../../i386/shared/comm/i386-stub-glue.c
libbsp_a_SOURCES += ../../i386/shared/comm/gdb_glue.c

# gnat
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c

include_HEADERS += ../../i386/shared/pci/pcibios.h
# pci
libbsp_a_SOURCES += ../../i386/shared/pci/pcibios.c \
    ../../i386/shared/pci/pcibios.h

include_HEADERS += ../../i386/shared/comm/uart.h
# startup
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += startup/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppretaskinghook.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspcmdline.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += ../../i386/shared/irq/idt.c
libbsp_a_SOURCES += ../../i386/shared/irq/irq.c
libbsp_a_SOURCES += ../../i386/shared/irq/irq_init.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspinit.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += startup/ldsegs.S
libbsp_a_SOURCES += ../../i386/shared/irq/irq_asm.S
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c

# timer
libbsp_a_SOURCES += timer/timer.c timer/timerisr.S

# ide
libbsp_a_SOURCES += ide/idecfg.c ide/ide.c

if HAS_SMP
libbsp_a_SOURCES += ../../i386/shared/smp/getcpuid.c 
libbsp_a_SOURCES += ../../i386/shared/smp/smp-imps.c 

project_lib_DATA += appstart.$(OBJEXT)
appcpustart.$(OBJEXT): start/start16.S
	$(CPPASCOMPILE) $(AM_CPPFLAGS) -DSMP_SECONDARY_CORE -o $@ -c $<

appstart.$(OBJEXT): appcpustart.$(OBJEXT)
	$(LD) -N -T $(top_srcdir)/startup/linkcmds \
	    -Ttext 0x70000 -e app_processor_start -nostdlib \
	    -o appstart_tmp.exe $<
	$(OBJCOPY) -O binary appstart_tmp.exe appstart.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 appstart.bin $@
endif

if HAS_NETWORKING
ne2000_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += ne2000.rel
ne2000_rel_SOURCES = ne2000/ne2000.c
ne2000_rel_CPPFLAGS = $(AM_CPPFLAGS) $(ne2000_CPPFLAGS)
ne2000_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if HAS_NETWORKING
# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
include_HEADERS += include/wd80x3.h

wd8003_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += wd8003.rel
wd8003_rel_SOURCES = wd8003/wd8003.c
wd8003_rel_SOURCES += include/wd80x3.h
wd8003_rel_CPPFLAGS = $(AM_CPPFLAGS) $(wd8003_CPPFLAGS)
wd8003_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if HAS_NETWORKING
3c509_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += 3c509.rel
3c509_rel_SOURCES = 3c509/3c509.c
3c509_rel_SOURCES += 3c509/3c509.h
3c509_rel_SOURCES += 3c509/elink.c
3c509_rel_SOURCES += 3c509/elink.h
3c509_rel_CPPFLAGS = $(AM_CPPFLAGS) $(3c509_CPPFLAGS)
3c509_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/cache.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/page.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/score.rel

if HAS_SMP
libbsp_a_LIBADD += appstart.$(OBJEXT)
endif

# We only build the Network library if HAS_NETWORKING was defined
# dec21140 is supported via libchip
if HAS_NETWORKING
libbsp_a_LIBADD += ne2000.rel
libbsp_a_LIBADD += wd8003.rel
libbsp_a_LIBADD += 3c509.rel
endif

EXTRA_DIST += HOWTO
EXTRA_DIST += README.dec21140
EXTRA_DIST += STATUS
EXTRA_DIST += times_i486dx
EXTRA_DIST += times_p5

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
