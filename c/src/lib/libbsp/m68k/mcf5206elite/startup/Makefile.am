## 
## $Id$
## 

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c m68kpretaskinghook.c bspstart.c \
    init5206e.c bootcard.c main.c sbrk.c setvec.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

$(PROJECT_RELEASE)/lib/gdbinit: gdbinit
	$(INSTALL_DATA) $< $@

bsplib_DATA = linkcmds gdbinit linkcmds.flash

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/linkcmds.flash: linkcmds.flash
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds \
    $(PROJECT_RELEASE)/lib/linkcmds.flash $(PROJECT_RELEASE)/lib/gdbinit

all: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c init5206e.c linkcmds linkcmds.flash \
    gdbinit

include $(top_srcdir)/../../../../automake/local.am
