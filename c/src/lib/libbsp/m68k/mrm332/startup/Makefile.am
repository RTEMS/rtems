##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = start_c.c bsplibc.c bsppost.c bspstart.c bspclean.c bootcard.c \
    m68kpretaskinghook.c main.c sbrk.c setvec.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = except_vect_332_ROM.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

bsplib_DATA = linkcmds linkcmds_ROM

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/linkcmds_ROM: linkcmds_ROM
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/except_vect_332_ROM$(LIB_VARIANT).$(OBJEXT): \
    $(ARCH)/except_vect_332_ROM.$(OBJEXT)
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds \
    $(PROJECT_RELEASE)/lib/linkcmds_ROM \
    $(PROJECT_RELEASE)/lib/except_vect_332_ROM$(LIB_VARIANT).$(OBJEXT)

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c except_vect_332_ROM.S linkcmds \
    linkcmds_ROM

include $(top_srcdir)/../../../../../../automake/local.am
