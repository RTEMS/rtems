##
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

# FIXME: We must not include *.cfg
include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

CLEANFILES =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

_SUBDIRS = . tools

TMPINSTALL_FILES =

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += start/start16.S
start16.$(OBJEXT): start/start16.S
	$(CPPASCOMPILE) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ -c $<

start16-elf32.$(OBJEXT): start16.$(OBJEXT)
	$(LD) -N -Ttext $(START16ADDR) -e start16 -nostdlib \
	  --oformat=elf32-i386 \
	-o $@ $<

start16.bin: start16-elf32.$(OBJEXT)
	$(OBJCOPY) -O binary $< $@
CLEANFILES += start16.bin

$(PROJECT_LIB)/start16.bin: start16.bin $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start16.bin
TMPINSTALL_FILES += $(PROJECT_LIB)/start16.bin

project_lib_DATA += start16.bin

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# clock
libbsp_a_SOURCES += clock/ckinit.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += clock/todcfg.c
libbsp_a_SOURCES += ../../shared/tod.c

if RTEMS_VGA
libbsp_a_SOURCES += console/inch.c
libbsp_a_SOURCES += console/outch.c
libbsp_a_SOURCES += console/defkeymap.c
libbsp_a_SOURCES += console/keyboard.c
libbsp_a_SOURCES += console/pc_keyb.c
libbsp_a_SOURCES += console/ps2_mouse.c
libbsp_a_SOURCES += console/vgainit.c
libbsp_a_SOURCES += console/vt.c
libbsp_a_SOURCES += console/videoAsm.S
libbsp_a_SOURCES += console/kbd_parser.c
libbsp_a_SOURCES += console/vgacons.c
if USE_VGA
libbsp_a_SOURCES += console/fb_vga.c
endif
if USE_CIRRUS_GD5446
libbsp_a_SOURCES += console/fb_cirrus.c
endif
if USE_VBE_RM
libbsp_a_SOURCES += console/fb_vesa_rm.c
endif
endif

# console (non-graphics support)
libbsp_a_SOURCES += console/serial_mouse_config.c
libbsp_a_SOURCES += ../../i386/shared/comm/uart.c
libbsp_a_SOURCES += ../../i386/shared/comm/tty_drv.c
libbsp_a_SOURCES += ../../i386/shared/realmode_int/realmode_int.c
libbsp_a_SOURCES += ../../shared/console.c
libbsp_a_SOURCES += console/console_select.c
libbsp_a_SOURCES += ../../shared/console_read.c
libbsp_a_SOURCES += ../../shared/console_write.c
libbsp_a_SOURCES += console/console_control.c
libbsp_a_SOURCES += console/conscfg.c
libbsp_a_SOURCES += console/printk_support.c
libbsp_a_SOURCES += console/exar17d15x.c
libbsp_a_SOURCES += console/rtd316.c
libbsp_a_SOURCES += console/uart_bus_pci.c
libbsp_a_SOURCES += console/gdb_select.c

# gdb
libbsp_a_SOURCES += ../../i386/shared/comm/i386-stub.c
libbsp_a_SOURCES += ../../i386/shared/comm/i386-stub-glue.c
libbsp_a_SOURCES += ../../i386/shared/comm/gdb_glue.c

# gnat
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c

# pci
libbsp_a_SOURCES += ../../i386/shared/pci/pcibios.c
libbsp_a_SOURCES += ../../i386/shared/pci/pci_io.c
libbsp_a_SOURCES += ../../shared/pci/pci_bus_count.c
libbsp_a_SOURCES += ../../shared/pci/pci_find_device.c

# startup
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += startup/bsp_fatal_halt.c
libbsp_a_SOURCES += startup/bspgetworkarea.c
libbsp_a_SOURCES += startup/bspidle.S
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspcmdline.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += ../../i386/shared/irq/idt.c
libbsp_a_SOURCES += ../../i386/shared/irq/irq.c
libbsp_a_SOURCES += ../../i386/shared/irq/irq_init.c
libbsp_a_SOURCES += ../../i386/shared/irq/elcr.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
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

if HAS_IDE
# ide
libbsp_a_SOURCES += ide/ide.c
libbsp_a_SOURCES += ide/idecfg.c
endif

if HAS_SMP
libbsp_a_SOURCES += ../../i386/shared/smp/getcpuid.c
libbsp_a_SOURCES += ../../i386/shared/smp/smp-imps.c

project_lib_DATA += appstart.$(OBJEXT)
appcpustart.$(OBJEXT): start/start16.S
	$(CPPASCOMPILE) $(AM_CPPFLAGS) -DSMP_SECONDARY_CORE -o $@ -c $<

appstart.$(OBJEXT): appcpustart.$(OBJEXT)
	$(LD) -r -N -T $(top_srcdir)/startup/linkcmds \
	    -Ttext 0x70000 -e app_processor_start -nostdlib \
	    -o appstart_tmp.exe $<
	$(OBJCOPY) -O binary appstart_tmp.exe appstart.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 appstart.bin $@
endif

if HAS_NETWORKING
libbsp_a_SOURCES += 3c509/3c509.c
libbsp_a_SOURCES += 3c509/3c509.h
libbsp_a_SOURCES += 3c509/elink.c
libbsp_a_SOURCES += 3c509/elink.h
libbsp_a_SOURCES += ne2000/ne2000.c
libbsp_a_SOURCES += wd8003/wd8003.c
endif

libbsp_a_SOURCES += ../../../../../../bsps/i386/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/cpuModel.S
libbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/displayCpu.c
libbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/idtr.S
libbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/page.c

if HAS_SMP
libbsp_a_LIBADD = appstart.$(OBJEXT)
endif

EXTRA_DIST += HOWTO
EXTRA_DIST += README.dec21140
EXTRA_DIST += STATUS
EXTRA_DIST += times_i486dx
EXTRA_DIST += times_p5

include $(top_srcdir)/../../../../automake/local.am
include $(top_srcdir)/../../../../automake/subdirs.am
include $(srcdir)/../../../../../../bsps/i386/pc386/headers.am
