##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bsplibc.c bsppost.c bspstart.c bspclean.c sbrk.c bootcard.c main.c \
    gnatinstallhandler.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

project_lib_DATA = linkcmds linkcmds.rom linkcmds.rom2ram

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(TMPINSTALL_FILES) $(PGM)

EXTRA_DIST = bspclean.c bspstart.c linkcmds linkcmds.rom linkcmds.rom2ram

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/linkcmds: linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/linkcmds.rom: linkcmds.rom $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.rom
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds.rom

$(PROJECT_LIB)/linkcmds.rom2ram: linkcmds.rom2ram $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.rom2ram
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds.rom2ram

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
