##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# FIXME: We must not include *.cfg
include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

SUBDIRS = . tools

include_HEADERS += include/crt.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start$(LIB_VARIANT).$(OBJEXT): start/start.S
	${CCASCOMPILE} -DASM -o $@ -c $<

project_lib_DATA = start$(LIB_VARIANT).$(OBJEXT)

EXTRA_DIST += start/start16.S
start16.$(OBJEXT): start/start16.S
	$(COMPILE.S) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ $<

start16$(LIB_VARIANT)-elf32.$(OBJEXT): start16.$(OBJEXT)
	$(LD) -N -T $(top_srcdir)/startup/linkcmds -Ttext $(START16ADDR) -e start16 -nostdlib \
	  --oformat=elf32-i386 \
	-o $@ $<

start16$(LIB_VARIANT).bin: start16$(LIB_VARIANT)-elf32.$(OBJEXT)
	$(OBJCOPY) -O binary $< $@

project_lib_DATA += start16$(LIB_VARIANT).bin

dist_project_lib_DATA += startup/linkcmds

EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/ckinit.c clock/rtc.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += clock$(LIB_VARIANT).rel

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = console/keyboard.h console/kd.h \
    console/serial_mouse.h console/ps2_drv.h console/fb_vga.h

EXTRA_PROGRAMS += console.rel
CLEANFILES += console.rel
console_rel_SOURCES = console/console.c console/inch.c console/outch.c \
    console/defkeymap.c console/fb_vga.c console/keyboard.c \
    console/mouse_parser.c console/pc_keyb.c console/ps2_mouse.c \
    console/serial_mouse.c console/vgainit.c console/vt.c \
    console/videoAsm.S
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += console$(LIB_VARIANT).rel

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspstart.c startup/exit.c ../../i386/shared/irq/idt.c \
    ../../i386/shared/irq/irq.c ../../i386/shared/irq/irq_init.c \
    ../../shared/bootcard.c ../../shared/main.c ../../shared/sbrk.c \
    ../../i386/shared/comm/i386-stub.c \
    ../../i386/shared/comm/i386-stub-glue.c ../../i386/shared/comm/uart.c \
    ../../i386/shared/pci/pcibios.c ../../shared/gnatinstallhandler.c \
    ../../i386/shared/comm/gdb_glue.c ../../i386/shared/comm/tty_drv.c \
    startup/ldsegs.S ../../i386/shared/irq/irq_asm.S
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += startup$(LIB_VARIANT).rel

EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += timer$(LIB_VARIANT).rel

if HAS_NETWORKING
ne2000_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
EXTRA_PROGRAMS += ne2000.rel
CLEANFILES += ne2000.rel
ne2000_rel_SOURCES = ne2000/ne2000.c
ne2000_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(ne2000_CPPFLAGS)
ne2000_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += ne2000_g.rel
CLEANFILES += ne2000_g.rel
ne2000_g_rel_SOURCES = $(ne2000_rel_SOURCES)
ne2000_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) $(ne2000_CPPFLAGS)
ne2000_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += ne2000$(LIB_VARIANT).rel
endif

if HAS_NETWORKING
# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
include_HEADERS += include/wd80x3.h

wd8003_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
EXTRA_PROGRAMS += wd8003.rel
CLEANFILES += wd8003.rel
wd8003_rel_SOURCES = wd8003/wd8003.c include/wd80x3.h
wd8003_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(wd8003_CPPFLAGS)
wd8003_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += wd8003_g.rel
CLEANFILES += wd8003_g.rel
wd8003_g_rel_SOURCES = $(wd8003_rel_SOURCES)
wd8003_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) $(wd8003_CPPFLAGS)
wd8003_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += wd8003$(LIB_VARIANT).rel
endif

if HAS_NETWORKING
3c509_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
EXTRA_PROGRAMS += 3c509.rel
CLEANFILES += 3c509.rel
3c509_rel_SOURCES = 3c509/3c509.c 3c509/3c509.h 3c509/elink.c 3c509/elink.h
3c509_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) $(3c509_CPPFLAGS)
3c509_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += 3c509_g.rel
CLEANFILES += 3c509_g.rel
3c509_g_rel_SOURCES = $(3c509_rel_SOURCES)
3c509_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) $(3c509_CPPFLAGS)
3c509_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += 3c509$(LIB_VARIANT).rel
endif

EXTRA_PROGRAMS += ide.rel
CLEANFILES += ide.rel
ide_rel_SOURCES = ide/idecfg.c ide/ide.c
ide_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
ide_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += ide_g.rel
CLEANFILES += ide_g.rel
ide_g_rel_SOURCES = $(ide_rel_SOURCES)
ide_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
ide_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += ide$(LIB_VARIANT).rel

EXTRA_LIBRARIES = libbsp.a
CLEANFILES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup$(LIB_VARIANT).rel clock$(LIB_VARIANT).rel \
    console$(LIB_VARIANT).rel timer$(LIB_VARIANT).rel ide$(LIB_VARIANT).rel
# We only build the Network library if HAS_NETWORKING was defined
# dec21140 is supported via libchip
if HAS_NETWORKING
libbsp_a_LIBADD += ne2000$(LIB_VARIANT).rel wd8003$(LIB_VARIANT).rel \
    3c509$(LIB_VARIANT).rel
endif
libbsp_a_LIBADD += ../../../libcpu/i386/cache$(LIB_VARIANT).rel \
    ../../../libcpu/i386/page$(LIB_VARIANT).rel \
    ../../../libcpu/i386/score$(LIB_VARIANT).rel

EXTRA_LIBRARIES += libbsp_g.a
CLEANFILES += libbsp_g.a
libbsp_g_a_SOURCES = $(libbsp_a_SOURCES)
libbsp_g_a_LIBADD = $(libbsp_a_LIBADD)

noinst_DATA += libbsp$(LIB_VARIANT).a

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST += HOWTO README.dec21140 STATUS times_i486dx times_p5

PREINSTALL_DIRS =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/crt.h: include/crt.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/crt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/crt.h

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/start16$(LIB_VARIANT).bin: start16$(LIB_VARIANT).bin $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start16$(LIB_VARIANT).bin
TMPINSTALL_FILES += $(PROJECT_LIB)/start16$(LIB_VARIANT).bin

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/clock$(LIB_VARIANT).rel: clock$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/clock$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/clock$(LIB_VARIANT).rel

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

$(PROJECT_LIB)/console$(LIB_VARIANT).rel: console$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/console$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/console$(LIB_VARIANT).rel

$(PROJECT_LIB)/startup$(LIB_VARIANT).rel: startup$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/startup$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/startup$(LIB_VARIANT).rel

$(PROJECT_LIB)/timer$(LIB_VARIANT).rel: timer$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/timer$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/timer$(LIB_VARIANT).rel

if HAS_NETWORKING
$(PROJECT_INCLUDE)/wd80x3.h: include/wd80x3.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/wd80x3.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/wd80x3.h
endif

$(PROJECT_LIB)/ide$(LIB_VARIANT).rel: ide$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/ide$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/ide$(LIB_VARIANT).rel

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
