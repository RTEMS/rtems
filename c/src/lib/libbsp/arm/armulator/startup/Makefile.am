##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bsplibc.c bsppost.c bspstart.c bootcard.c main.c sbrk.c \
    gnatinstallhandler.c libcfunc.c syscalls.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = trap.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#
# From newlib
# Select which debug protocol is being used.
# ARM_RDP_MONITOR selects the Demon monitor.
# ARM_RDI_MONITOR selects the Angel monitor.
# If neither are defined, then hard coded defaults will be used
# to create the program's environment.
AM_CPPFLAGS += -DARM_RDI_MONITOR

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspclean.c bspstart.c exit.c linkcmds setvec.c

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/linkcmds: linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

include $(top_srcdir)/../../../../automake/local.am
