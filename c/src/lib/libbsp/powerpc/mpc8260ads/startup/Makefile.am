##
## $Id$
##

PGM = $(ARCH)/startup.rel

C_FILES = bspstart.c cpuinit.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

shared_C_FILES = $(top_srcdir)/../../shared/bootcard.c
shared_C_FILES += $(top_srcdir)/../../shared/bspclean.c
shared_C_FILES += $(top_srcdir)/../../shared/bsplibc.c
shared_C_FILES += $(top_srcdir)/../../shared/bsppost.c
shared_C_FILES += $(top_srcdir)/../../shared/main.c
shared_C_FILES += $(top_srcdir)/../../shared/sbrk.c
shared_C_FILES += $(top_srcdir)/../../shared/gnatinstallhandler.c
OBJS += \
    $(shared_C_FILES:$(top_srcdir)/../../shared/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../shared/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = linkcmds $(C_FILES)

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
