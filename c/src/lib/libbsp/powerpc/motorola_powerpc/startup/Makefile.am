##
## $Id$
##

PGM = $(ARCH)/startup.rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

powerpc_shared_startup_C_FILES = \
    $(top_srcdir)/../../powerpc/shared/startup/bspstart.c \
    $(top_srcdir)/../../powerpc/shared/startup/pgtbl_setup.c \
    $(top_srcdir)/../../powerpc/shared/startup/pgtbl_activate.c \
    $(top_srcdir)/../../powerpc/shared/startup/sbrk.c
OBJS = \
    $(powerpc_shared_startup_C_FILES:$(top_srcdir)/../../powerpc/shared/startup/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/startup/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

shared_C_FILES = $(top_srcdir)/../../shared/bootcard.c
shared_C_FILES += $(top_srcdir)/../../shared/bspclean.c
shared_C_FILES += $(top_srcdir)/../../shared/bsplibc.c
shared_C_FILES += $(top_srcdir)/../../shared/bsppost.c
shared_C_FILES += $(top_srcdir)/../../shared/main.c
shared_C_FILES += $(top_srcdir)/../../shared/gnatinstallhandler.c
OBJS += \
    $(shared_C_FILES:$(top_srcdir)/../../shared/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../shared/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = linkcmds

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/linkcmds: linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

include $(top_srcdir)/../../../../automake/local.am
