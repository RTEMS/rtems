##

VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bsplibc.c bsppost.c bspstart.c exit.c bootcard.c main.c sbrk.c \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspstart.c exit.c linkcmds

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/linkcmds: linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
