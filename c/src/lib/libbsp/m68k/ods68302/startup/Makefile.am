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

project_lib_DATA = linkcmds rom debugger

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspclean.c bspstart.c crc.c debugger debugport.c gdb-hooks.c \
    linkcmds m68302scc.c m68k-stub.c memcheck.c rom trace.c

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

${PROJECT_RELEASE}/lib/rom: rom ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/rom
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/rom

${PROJECT_RELEASE}/lib/debugger: debugger ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/debugger
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/debugger

include $(top_srcdir)/../../../../automake/local.am
