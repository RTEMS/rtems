##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared:@srcdir@/../../shared/gdbstub::@srcdir@/../../../shared/gdbstub

PGM = $(ARCH)/startup.rel

C_FILES = alloc360.c bspclean.c bsplibc.c bsppost.c m68kpretaskinghook.c \
    bspstart.c init68360.c bootcard.c main.c sbrk.c setvec.c \
    gnatinstallhandler.c m68k-stub.c rtems-stub-glue.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -I@srcdir@/../../shared/gdbstub

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds linkcmds.prom linkcmds.bootp

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = alloc360.c bspclean.c bspstart.c init68360.c linkcmds \
    linkcmds.bootp linkcmds.prom

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/linkcmds: linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/linkcmds.prom: linkcmds.prom $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.prom
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds.prom

$(PROJECT_LIB)/linkcmds.bootp: linkcmds.bootp $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds.bootp
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds.bootp

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
