##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c main.c sbrk.c \
    setvec.c inittlb.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = idtmem.S idttlb.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds ghlinkcmds

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspclean.c bspstart.c ghlinkcmds idtmem.S idttlb.S inittlb.c \
    linkcmds setvec.c

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/linkcmds: linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

$(PROJECT_LIB)/ghlinkcmds: ghlinkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/ghlinkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/ghlinkcmds

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
