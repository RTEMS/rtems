##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

powerpc_shared_start_S_FILES = \
    $(top_srcdir)/../../powerpc/shared/start/start.S \
    $(top_srcdir)/../../powerpc/shared/start/rtems_crti.S

OBJS = \
    $(powerpc_shared_start_S_FILES:$(top_srcdir)/../../powerpc/shared/start/%.S=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/start/%.S $(ARCH)/$(dirstamp)
	${CCASCOMPILE} -o $@ -c $<

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)
project_lib_DATA += $(ARCH)/rtems_crti$(LIB_VARIANT).$(OBJEXT)

all-local: $(TMPINSTALL_FILES)

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT): $(ARCH)/rtems_crti$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/rtems_crti$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/local.am
