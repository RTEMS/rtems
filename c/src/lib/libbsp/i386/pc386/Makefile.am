##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# FIXME: We must not include *.cfg
include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

SUBDIRS = . tools

include_HEADERS += include/crt.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += start/start16.S
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

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c clock/todcfg.c ../../shared/tod.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = console/keyboard.h console/kd.h \
    console/serial_mouse.h console/ps2_drv.h console/fb_vga.h

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c console/inch.c console/outch.c \
    console/defkeymap.c console/fb_vga.c console/keyboard.c \
    console/mouse_parser.c console/pc_keyb.c console/ps2_mouse.c \
    console/serial_mouse.c console/vgainit.c console/vt.c console/videoAsm.S \
    ../../i386/shared/comm/uart.c ../../i386/shared/comm/tty_drv.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += gdb.rel
gdb_rel_SOURCES = ../../i386/shared/comm/i386-stub.c \
    ../../i386/shared/comm/i386-stub-glue.c \
    ../../i386/shared/comm/gdb_glue.c
gdb_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
gdb_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += gnat.rel
gnat_rel_SOURCES = ../../shared/gnatinstallhandler.c
gnat_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
gnat_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += pci.rel
pci_rel_SOURCES = ../../i386/shared/pci/pcibios.c
pci_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspstart.c startup/exit.c ../../i386/shared/irq/idt.c \
    ../../i386/shared/irq/irq.c ../../i386/shared/irq/irq_init.c \
    ../../shared/bootcard.c ../../shared/main.c ../../shared/sbrk.c \
    startup/ldsegs.S ../../i386/shared/irq/irq_asm.S
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
ne2000_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += ne2000.rel
ne2000_rel_SOURCES = ne2000/ne2000.c
ne2000_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(ne2000_CPPFLAGS)
ne2000_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if HAS_NETWORKING
# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
include_HEADERS += include/wd80x3.h

wd8003_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += wd8003.rel
wd8003_rel_SOURCES = wd8003/wd8003.c include/wd80x3.h
wd8003_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(wd8003_CPPFLAGS)
wd8003_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if HAS_NETWORKING
3c509_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += 3c509.rel
3c509_rel_SOURCES = 3c509/3c509.c 3c509/3c509.h 3c509/elink.c 3c509/elink.h
3c509_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) $(3c509_CPPFLAGS)
3c509_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_PROGRAMS += ide.rel
ide_rel_SOURCES = ide/idecfg.c ide/ide.c
ide_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
ide_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel clock.rel console.rel timer.rel ide.rel \
    gdb.rel gnat.rel pci.rel
# We only build the Network library if HAS_NETWORKING was defined
# dec21140 is supported via libchip
if HAS_NETWORKING
libbsp_a_LIBADD += ne2000.rel wd8003.rel 3c509.rel
endif
libbsp_a_LIBADD += ../../../libcpu/i386/cache.rel \
    ../../../libcpu/i386/page.rel ../../../libcpu/i386/score.rel

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST += HOWTO README.dec21140 STATUS times_i486dx times_p5

PREINSTALL_DIRS =
PREINSTALL_FILES =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/bsp_specs: bsp_specs $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/bsp_specs
PREINSTALL_FILES += $(PROJECT_LIB)/bsp_specs

$(PROJECT_INCLUDE)/bsp.h: include/bsp.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp.h

$(PROJECT_INCLUDE)/tm27.h: include/tm27.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tm27.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tm27.h

$(PROJECT_INCLUDE)/bspopts.h: include/bspopts.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bspopts.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bspopts.h

$(PROJECT_INCLUDE)/crt.h: include/crt.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/crt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/crt.h

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start.$(OBJEXT): start.$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start.$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start.$(OBJEXT)

$(PROJECT_LIB)/start16.bin: start16.bin $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start16.bin
TMPINSTALL_FILES += $(PROJECT_LIB)/start16.bin

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
PREINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/keyboard.h: console/keyboard.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/keyboard.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/keyboard.h

$(PROJECT_INCLUDE)/rtems/kd.h: console/kd.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/kd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/kd.h

$(PROJECT_INCLUDE)/rtems/serial_mouse.h: console/serial_mouse.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/serial_mouse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/serial_mouse.h

$(PROJECT_INCLUDE)/rtems/ps2_drv.h: console/ps2_drv.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/ps2_drv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ps2_drv.h

$(PROJECT_INCLUDE)/rtems/fb_vga.h: console/fb_vga.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/fb_vga.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/fb_vga.h

if HAS_NETWORKING
$(PROJECT_INCLUDE)/wd80x3.h: include/wd80x3.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/wd80x3.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/wd80x3.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
