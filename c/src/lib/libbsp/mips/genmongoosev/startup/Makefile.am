##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared:@srcdir@/../../shared/gdbstub:@srcdir@/../../../shared/gdbstub

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c main.c sbrk.c \
    gnatinstallhandler.c setvec.c mips-stub.c gdb-support.c \
    rtems-stub-glue.c
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
