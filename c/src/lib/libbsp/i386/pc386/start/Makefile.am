##
## $Id$
##

S_FILES = start16.S start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

#
# (OPTIONAL) Add local stuff here using +=
#

LINKCMDS = $(top_srcdir)/startup/linkcmds

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)
project_lib_DATA += ${PROJECT_RELEASE}/lib/start16$(LIB_VARIANT).bin

$(ARCH)/start16.$(OBJEXT): start16.S $(ARCH)/$(dirstamp)
	$(COMPILE.S) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ $<

$(ARCH)/start16.bin: $(ARCH)/start16.$(OBJEXT) $(ARCH)/$(dirstamp)
	$(LD) -N -T $(LINKCMDS) -Ttext $(START16ADDR) -e start16 -nostdlib \
	  --oformat=elf32-i386 \
	-o $(basename $@)-elf32.$(OBJEXT) $(basename $@).$(OBJEXT)
	$(OBJCOPY) -O binary $(basename $@)-elf32.$(OBJEXT) $@

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = start.S start16.S

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/start16$(LIB_VARIANT).bin: ${PROJECT_RELEASE}/lib/start16$(LIB_VARIANT).bin $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start16$(LIB_VARIANT).bin
TMPINSTALL_FILES += $(PROJECT_LIB)/start16$(LIB_VARIANT).bin

include $(top_srcdir)/../../../../automake/local.am
