##
## $Id$
##

PGM = $(ARCH)/console.rel

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = keyboard.h kd.h serial_mouse.h ps2_drv.h fb_vga.h

C_FILES = console.c inch.c outch.c defkeymap.c fb_vga.c keyboard.c \
    mouse_parser.c pc_keyb.c ps2_mouse.c serial_mouse.c vgainit.c vt.c
S_FILES = videoAsm.S

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) \
    $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

all-local: $(PREINSTALL_FILES) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = $(C_FILES) $(S_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/keyboard.h: keyboard.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/keyboard.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/keyboard.h

$(PROJECT_INCLUDE)/rtems/kd.h: kd.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/kd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/kd.h

$(PROJECT_INCLUDE)/rtems/serial_mouse.h: serial_mouse.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/serial_mouse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/serial_mouse.h

$(PROJECT_INCLUDE)/rtems/ps2_drv.h: ps2_drv.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/ps2_drv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ps2_drv.h

$(PROJECT_INCLUDE)/rtems/fb_vga.h: fb_vga.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/fb_vga.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/fb_vga.h

include $(top_srcdir)/../../../../automake/local.am
