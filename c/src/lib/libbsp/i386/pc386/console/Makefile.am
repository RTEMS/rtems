##
## $Id$
##


PGM = $(ARCH)/console.rel

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = keyboard.h kd.h serial_mouse.h ps2_drv.h fb_vga.h

C_FILES = console.c inch.c outch.c defkeymap.c fb_vga.c keyboard.c \
    mouse_parser.c pc_keyb.c ps2_mouse.c serial_mouse.c vgainit.c vt.c
S_FILES = videoAsm.S

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

all-local: $(PREINSTALL_FILES) $(ARCH) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
