##
## $Id$
##

C_FILES = start.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

bsplib_DATA = $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(ARCH)/start.o
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/except_vect_332_ROM$(LIB_VARIANT).o: \
    $(ARCH)/except_vect_332_ROM.o
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/ram_init$(LIB_VARIANT): \
    $(ARCH)/ram_init
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/ram_init_FW$(LIB_VARIANT): \
    $(ARCH)/ram_init_FW
	$(INSTALL_DATA) $< $@

$(ARCH)/ram_init: $(srcdir)/start.c $(srcdir)/ram_init.ld \
    $(srcdir)/ram_init.sed
	sed -f $(srcdir)/ram_init.sed $(srcdir)/start.c >$(ARCH)/ram_init.S
	$(CC) -D SIM_CRB=0x0000 -E $(ARCH)/ram_init.S >$(ARCH)/ram_init1.S
	sed "/.org/ {N; s/\n/%/; P;}; /\.text/P; D;" $(ARCH)/ram_init1.S | \
           sort | sed "h;1{s/.*/.data/;P;g;};s/%.*//;P;g;s/.*%//;" \
           >$(ARCH)/ram_init2.S
	$(CC) -nostdlib -Wl,-T$(srcdir)/ram_init.ld $(ARCH)/ram_init2.S \
           -o $(ARCH)/ram_init

$(ARCH)/ram_init_FW: $(srcdir)/ram_init.ld $(ARCH)/ram_init.S
	$(CC) -D FLASHWRITE -D SIM_CRB=0x0000 -E $(ARCH)/ram_init.S \
           >$(ARCH)/ram_init_FW1.S
	sed "/.org/ {N; s/\n/%/; P;}; /\.text/P; D;" $(ARCH)/ram_init_FW1.S | \
           sort | sed "h;1{s/.*/.data/;P;g;};s/%.*//;P;g;s/.*%//;" \
           >$(ARCH)/ram_init_FW2.S
	$(CC) -nostdlib -Wl,-T$(srcdir)/ram_init.ld $(ARCH)/ram_init_FW2.S \
           -o $(ARCH)/ram_init_FW

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o \
    $(PROJECT_RELEASE)/lib/ram_init$(LIB_VARIANT) \
    $(PROJECT_RELEASE)/lib/ram_init_FW$(LIB_VARIANT) \
    $(PROJECT_RELEASE)/lib/except_vect_332_ROM$(LIB_VARIANT).o

all-local: $(ARCH) $(OBJS) $(ARCH)/start.o $(TMPINSTALL_FILES)

.PRECIOUS: $(ARCH)/start.o

EXTRA_DIST = start.c except_vect_332_ROM.S ram_init.ld ram_init.sed

include $(top_srcdir)/../../../../../../automake/local.am
