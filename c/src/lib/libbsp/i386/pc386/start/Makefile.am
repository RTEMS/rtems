##
## $Id$
##

S_FILES = start16.S start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

LINKCMDS = $(top_srcdir)/startup/linkcmds

START_O_FILES = $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)
START_O_FILES += $(PROJECT_RELEASE)/lib/start16$(LIB_VARIANT).bin

install-data-local: $(START_O_FILES)
	@$(mkinstalldirs) $(DESTDIR)$(bsplibdir)
	list='$(START_O_FILES)'; for i in $$list; do \
	$(INSTALL_DATA) $$i $(DESTDIR)$(bsplibdir); \
	done

$(ARCH)/start16.$(OBJEXT): start16.S
	$(COMPILE.S) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ $<

$(ARCH)/start16.bin: $(ARCH)/start16.$(OBJEXT)
	$(LD) -N -T $(LINKCMDS) -Ttext $(START16ADDR) -e start16 -nostdlib 	  --oformat=elf32-i386 -o $(basename $@).$(OBJEXT)bj $(basename $@).$(OBJEXT)
	$(OBJCOPY) -O binary $(basename $@).$(OBJEXT)bj $@

$(PROJECT_RELEASE)/lib/start16$(LIB_VARIANT).bin: $(ARCH)/start16.bin
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start.$(OBJEXT)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/start16$(LIB_VARIANT).bin \
    $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = start.S start16.S

include $(top_srcdir)/../../../../automake/local.am
