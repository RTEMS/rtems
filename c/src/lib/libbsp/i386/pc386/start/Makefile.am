##
## $Id$
##


PGMS = $(ARCH)/start.o $(ARCH)/start16.bin

S_FILES = start16.S start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

LINKCMDS = $(top_srcdir)/startup/linkcmds

$(ARCH)/start16.o: start16.S
	$(COMPILE.S) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ $<

$(ARCH)/start16.bin: $(ARCH)/start16.o
	$(LD) -N -T $(LINKCMDS) -Ttext $(START16ADDR) -e start16 -nostdlib 	  --oformat=elf32-i386 -o $(basename $@).obj $(basename $@).o
	$(OBJCOPY) -O binary $(basename $@).obj $@

$(PROJECT_RELEASE)/lib/start16$(LIB_VARIANT).bin: $(ARCH)/start16.bin
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(ARCH)/start.o
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start16$(LIB_VARIANT).bin \
    $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = start.S start16.S

include $(top_srcdir)/../../../../../../automake/local.am
