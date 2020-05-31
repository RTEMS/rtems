##
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# FIXME: We must not include *.cfg
include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

CLEANFILES =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/i386/pc386/start/bsp_specs


noinst_PROGRAMS =

start.$(OBJEXT): ../../../../../../bsps/i386/pc386/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/i386/pc386/clock/ckinit.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/i386/pc386/clock/todcfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c

if RTEMS_VGA
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/inch.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/outch.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/defkeymap.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/keyboard.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/pc_keyb.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/ps2_mouse.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/vgainit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/vt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/videoAsm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/kbd_parser.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/vgacons.c
if USE_VGA
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/fb_vga.c
endif
if USE_CIRRUS_GD5446
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/fb_cirrus.c
endif
if USE_VBE_RM
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/fb_vesa_rm.c
endif
endif

# console (non-graphics support)
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/serial_mouse_config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/tty_drv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/realmode_int.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/console_select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/console_control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/conscfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/printk_support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/exar17d15x.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/rtd316.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/uart_bus_pci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/gdb_select.c

# gdb
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/i386-stub.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/i386-stub-glue.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/console/gdb_glue.c

# gnat

# pci
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/pci/pcibios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/pci/pci_io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/pci/pci_bus_count.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/pci/pci_find_device.c

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/bsp_fatal_halt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/bspidle.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/bspcmdline.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/irq/idt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/irq/irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/irq/irq_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/irq/elcr.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/ldsegs.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/irq/irq_asm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/btimer/btimer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/btimer/timerisr.S

if HAS_IDE
# ide
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/ata/ide.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/ata/idecfg.c
endif

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/smp-imps.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/bspsmp.c

project_lib_DATA += appstart.$(OBJEXT)
appcpustart.$(OBJEXT): ../../../../../../bsps/i386/pc386/start/startAP.S
	$(CPPASCOMPILE) $(AM_CPPFLAGS) -o $@ -c $<

appstart.$(OBJEXT): appcpustart.$(OBJEXT)
	$(LD) -N  \
	    -Ttext 0x70000 -e app_processor_start -nostdlib \
	    -o appstart_tmp.exe $<
	$(OBJCOPY) -O binary appstart_tmp.exe appstart.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 appstart.bin $@
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/net/3c509.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/net/elink.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/net/ne2000.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/net/wd8003.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/cpuModel.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/displayCpu.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/idtr.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/page.c

if HAS_SMP
librtemsbsp_a_LIBADD = appstart.$(OBJEXT)
endif


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(top_srcdir)/../../../../automake/subdirs.am
include $(srcdir)/../../../../../../bsps/i386/pc386/headers.am
