##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# FIXME: We must not include *.cfg
include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools startup clock console timer ne2000 wd8003 3c509 ide \
    wrapup

# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
if HAS_NETWORKING
include_HEADERS += include/wd80x3.h
endif

include_HEADERS += include/crt.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h

S_FILES = start/start.S
$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): start/start.S $(ARCH)/$(dirstamp)
	${CCASCOMPILE} -DASM -o $@ -c $<

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

S_FILES += start/start16.S
$(ARCH)/start16.$(OBJEXT): start/start16.S $(ARCH)/$(dirstamp)
	$(COMPILE.S) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ $<

$(ARCH)/start16$(LIB_VARIANT)-elf32.$(OBJEXT): $(ARCH)/start16.$(OBJEXT) $(ARCH)/$(dirstamp)
	$(LD) -N -T $(top_srcdir)/startup/linkcmds -Ttext $(START16ADDR) -e start16 -nostdlib \
	  --oformat=elf32-i386 \
	-o $@ $<

$(ARCH)/start16$(LIB_VARIANT).bin: $(ARCH)/start16$(LIB_VARIANT)-elf32.$(OBJEXT)
	$(OBJCOPY) -O binary $< $@

project_lib_DATA += $(ARCH)/start16$(LIB_VARIANT).bin

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST = HOWTO README.dec21140 STATUS times_i486dx times_p5

PREINSTALL_DIRS =
TMPINSTALL_FILES =

if HAS_NETWORKING
$(PROJECT_INCLUDE)/wd80x3.h: include/wd80x3.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/wd80x3.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/wd80x3.h
endif

$(PROJECT_INCLUDE)/crt.h: include/crt.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/crt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/crt.h

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/start16$(LIB_VARIANT).bin: $(ARCH)/start16$(LIB_VARIANT).bin $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start16$(LIB_VARIANT).bin
TMPINSTALL_FILES += $(PROJECT_LIB)/start16$(LIB_VARIANT).bin

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
