##
## $Id$
##

C_FILES = start.c
OBJS = $(C_FILES:%.c=$(ARCH)/%$(LIB_VARIANT).$(OBJEXT))
S_FILES = except_vect_332_ROM.S
OBJS += $(S_FILES:%.c=$(ARCH)/%$(LIB_VARIANT).$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

#
# (OPTIONAL) Add local stuff here using +=
#

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)
project_lib_DATA += $(ARCH)/except_vect_332_ROM$(LIB_VARIANT).$(OBJEXT)
project_lib_DATA += $(ARCH)/ram_init$(LIB_VARIANT)
project_lib_DATA += $(ARCH)/ram_init_FW$(LIB_VARIANT)

$(ARCH)/ram_init$(LIB_VARIANT): $(srcdir)/start.c $(srcdir)/ram_init.ld $(srcdir)/ram_init.sed $(ARCH)/$(dirstamp)
	sed -f $(srcdir)/ram_init.sed $(srcdir)/start.c >$(ARCH)/ram_init.S
	$(CC) -D SIM_CRB=0x0000 -E $(ARCH)/ram_init.S >$(ARCH)/ram_init1.S
	sed "/.$(OBJEXT)rg/ {N; s/\n/%/; P;}; /\.text/P; D;" $(ARCH)/ram_init1.S | \
           sort | sed "h;1{s/.*/.data/;P;g;};s/%.*//;P;g;s/.*%//;" \
           >$(ARCH)/ram_init2.S
	$(CC) -nostdlib -Wl,-T$(srcdir)/ram_init.ld $(ARCH)/ram_init2.S \
           -o $(ARCH)/ram_init$(LIB_VARIANT)

$(ARCH)/ram_init_FW$(LIB_VARIANT): $(srcdir)/ram_init.ld $(ARCH)/ram_init.S $(ARCH)/$(dirstamp)
	$(CC) -D FLASHWRITE -D SIM_CRB=0x0000 -E $(ARCH)/ram_init.S \
           >$(ARCH)/ram_init_FW1.S
	sed "/.$(OBJEXT)rg/ {N; s/\n/%/; P;}; /\.text/P; D;" $(ARCH)/ram_init_FW1.S | \
           sort | sed "h;1{s/.*/.data/;P;g;};s/%.*//;P;g;s/.*%//;" \
           >$(ARCH)/ram_init_FW2.S
	$(CC) -nostdlib -Wl,-T$(srcdir)/ram_init.ld $(ARCH)/ram_init_FW2.S \
           -o $(ARCH)/ram_init_FW$(LIB_VARIANT)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = start.c except_vect_332_ROM.S ram_init.ld ram_init.sed

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/except_vect_332_ROM$(LIB_VARIANT).$(OBJEXT): $(ARCH)/except_vect_332_ROM$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/except_vect_332_ROM$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/except_vect_332_ROM$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/ram_init$(LIB_VARIANT): $(ARCH)/ram_init$(LIB_VARIANT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/ram_init$(LIB_VARIANT)
TMPINSTALL_FILES += $(PROJECT_LIB)/ram_init$(LIB_VARIANT)

$(PROJECT_LIB)/ram_init_FW$(LIB_VARIANT): $(ARCH)/ram_init_FW$(LIB_VARIANT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/ram_init_FW$(LIB_VARIANT)
TMPINSTALL_FILES += $(PROJECT_LIB)/ram_init_FW$(LIB_VARIANT)

include $(top_srcdir)/../../../../automake/local.am
