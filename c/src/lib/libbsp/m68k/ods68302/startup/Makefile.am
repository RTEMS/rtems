##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = cpuboot.c crc.c debugport.c gdb-hooks.c bootcard.c main.c \
    m68302scc.c m68k-stub.c m68kpretaskinghook.c memcheck.c trace.c \
    gnatinstallhandler.c bsplibc.c bsppost.c bspstart.c bspclean.c sbrk.c \
    setvec.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

bsplib_DATA = linkcmds rom debugger

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/rom: rom
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/debugger: debugger
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds \
    $(PROJECT_RELEASE)/lib/rom $(PROJECT_RELEASE)/lib/debugger

all-local: $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c crc.c debugger debugport.c gdb-hooks.c \
    linkcmds m68302scc.c m68k-stub.c memcheck.c rom trace.c

include $(top_srcdir)/../../../../automake/local.am
