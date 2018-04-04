##
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

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

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

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

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# clock
librtemsbsp_a_SOURCES += clock/ckinit.c
librtemsbsp_a_SOURCES += ../../shared/clockdrv_shell.h
librtemsbsp_a_SOURCES += clock/todcfg.c
librtemsbsp_a_SOURCES += ../../shared/tod.c

if RTEMS_VGA
librtemsbsp_a_SOURCES += console/inch.c
librtemsbsp_a_SOURCES += console/outch.c
librtemsbsp_a_SOURCES += console/defkeymap.c
librtemsbsp_a_SOURCES += console/keyboard.c
librtemsbsp_a_SOURCES += console/pc_keyb.c
librtemsbsp_a_SOURCES += console/ps2_mouse.c
librtemsbsp_a_SOURCES += console/vgainit.c
librtemsbsp_a_SOURCES += console/vt.c
librtemsbsp_a_SOURCES += console/videoAsm.S
librtemsbsp_a_SOURCES += console/kbd_parser.c
librtemsbsp_a_SOURCES += console/vgacons.c
if USE_VGA
librtemsbsp_a_SOURCES += console/fb_vga.c
endif
if USE_CIRRUS_GD5446
librtemsbsp_a_SOURCES += console/fb_cirrus.c
endif
if USE_VBE_RM
librtemsbsp_a_SOURCES += console/fb_vesa_rm.c
endif
endif

# console (non-graphics support)
librtemsbsp_a_SOURCES += console/serial_mouse_config.c
librtemsbsp_a_SOURCES += ../../i386/shared/comm/uart.c
librtemsbsp_a_SOURCES += ../../i386/shared/comm/tty_drv.c
librtemsbsp_a_SOURCES += ../../i386/shared/realmode_int/realmode_int.c
librtemsbsp_a_SOURCES += ../../shared/console.c
librtemsbsp_a_SOURCES += console/console_select.c
librtemsbsp_a_SOURCES += ../../shared/console_read.c
librtemsbsp_a_SOURCES += ../../shared/console_write.c
librtemsbsp_a_SOURCES += console/console_control.c
librtemsbsp_a_SOURCES += console/conscfg.c
librtemsbsp_a_SOURCES += console/printk_support.c
librtemsbsp_a_SOURCES += console/exar17d15x.c
librtemsbsp_a_SOURCES += console/rtd316.c
librtemsbsp_a_SOURCES += console/uart_bus_pci.c
librtemsbsp_a_SOURCES += console/gdb_select.c

# gdb
librtemsbsp_a_SOURCES += ../../i386/shared/comm/i386-stub.c
librtemsbsp_a_SOURCES += ../../i386/shared/comm/i386-stub-glue.c
librtemsbsp_a_SOURCES += ../../i386/shared/comm/gdb_glue.c

# gnat
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c

# pci
librtemsbsp_a_SOURCES += ../../i386/shared/pci/pcibios.c
librtemsbsp_a_SOURCES += ../../i386/shared/pci/pci_io.c
librtemsbsp_a_SOURCES += ../../shared/pci/pci_bus_count.c
librtemsbsp_a_SOURCES += ../../shared/pci/pci_find_device.c

# startup
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += startup/bsp_fatal_halt.c
librtemsbsp_a_SOURCES += startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += startup/bspidle.S
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspcmdline.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += ../../i386/shared/irq/idt.c
librtemsbsp_a_SOURCES += ../../i386/shared/irq/irq.c
librtemsbsp_a_SOURCES += ../../i386/shared/irq/irq_init.c
librtemsbsp_a_SOURCES += ../../i386/shared/irq/elcr.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += startup/ldsegs.S
librtemsbsp_a_SOURCES += ../../i386/shared/irq/irq_asm.S
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c

# timer
librtemsbsp_a_SOURCES += timer/timer.c timer/timerisr.S

if HAS_IDE
# ide
librtemsbsp_a_SOURCES += ide/ide.c
librtemsbsp_a_SOURCES += ide/idecfg.c
endif

if HAS_SMP
librtemsbsp_a_SOURCES += ../../i386/shared/smp/getcpuid.c
librtemsbsp_a_SOURCES += ../../i386/shared/smp/smp-imps.c

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
librtemsbsp_a_SOURCES += 3c509/3c509.c
librtemsbsp_a_SOURCES += 3c509/3c509.h
librtemsbsp_a_SOURCES += 3c509/elink.c
librtemsbsp_a_SOURCES += 3c509/elink.h
librtemsbsp_a_SOURCES += ne2000/ne2000.c
librtemsbsp_a_SOURCES += wd8003/wd8003.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/cpuModel.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/displayCpu.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/idtr.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/start/page.c

if HAS_SMP
librtemsbsp_a_LIBADD = appstart.$(OBJEXT)
endif


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(top_srcdir)/../../../../automake/subdirs.am
include $(srcdir)/../../../../../../bsps/i386/pc386/headers.am
