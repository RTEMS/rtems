##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

sapi_C_FILES = sapi/no-ext.c sapi/no-io.c
OBJS = $(sapi_C_FILES:sapi/%.c=$(ARCH)/%.$(OBJEXT))

rtems_C_FILES = rtems/no-dpmem.c rtems/no-event.c rtems/no-msg.c \
    rtems/no-mp.c rtems/no-part.c rtems/no-region.c rtems/no-rtmon.c \
    rtems/no-sem.c rtems/no-signal.c rtems/no-timer.c
OBJS += $(rtems_C_FILES:rtems/%.c=$(ARCH)/%.$(OBJEXT))

PGMS = $(rtems_C_FILES:rtems/%.c=$(ARCH)/%$(LIB_VARIANT).rel)
PGMS += $(sapi_C_FILES:sapi/%.c=$(ARCH)/%$(LIB_VARIANT).rel)

$(ARCH)/%.$(OBJEXT): sapi/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

$(ARCH)/%.$(OBJEXT): rtems/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

# Make foo.rel from foo.o
$(ARCH)/%$(LIB_VARIANT).rel: $(ARCH)/%.o
	${make-rel}

project_lib_DATA = $(ARCH)/no-ext$(LIB_VARIANT).rel \
    $(ARCH)/no-io$(LIB_VARIANT).rel $(ARCH)/no-dpmem$(LIB_VARIANT).rel \
    $(ARCH)/no-event$(LIB_VARIANT).rel $(ARCH)/no-msg$(LIB_VARIANT).rel \
    $(ARCH)/no-mp$(LIB_VARIANT).rel $(ARCH)/no-part$(LIB_VARIANT).rel \
    $(ARCH)/no-region$(LIB_VARIANT).rel $(ARCH)/no-rtmon$(LIB_VARIANT).rel \
    $(ARCH)/no-sem$(LIB_VARIANT).rel $(ARCH)/no-signal$(LIB_VARIANT).rel \
    $(ARCH)/no-timer$(LIB_VARIANT).rel

all-local: $(PGMS) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGMS) $(OBJS)

EXTRA_DIST = $(rtems_C_FILES) $(sapi_C_FILES)

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/no-ext$(LIB_VARIANT).rel: $(ARCH)/no-ext$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-ext$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-ext$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-io$(LIB_VARIANT).rel: $(ARCH)/no-io$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-io$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-io$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-dpmem$(LIB_VARIANT).rel: $(ARCH)/no-dpmem$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-dpmem$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-dpmem$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-event$(LIB_VARIANT).rel: $(ARCH)/no-event$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-event$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-event$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-msg$(LIB_VARIANT).rel: $(ARCH)/no-msg$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-msg$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-msg$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-mp$(LIB_VARIANT).rel: $(ARCH)/no-mp$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-mp$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-mp$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-part$(LIB_VARIANT).rel: $(ARCH)/no-part$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-part$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-part$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-region$(LIB_VARIANT).rel: $(ARCH)/no-region$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-region$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-region$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-rtmon$(LIB_VARIANT).rel: $(ARCH)/no-rtmon$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-rtmon$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-rtmon$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-sem$(LIB_VARIANT).rel: $(ARCH)/no-sem$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-sem$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-sem$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-signal$(LIB_VARIANT).rel: $(ARCH)/no-signal$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-signal$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-signal$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-timer$(LIB_VARIANT).rel: $(ARCH)/no-timer$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-timer$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-timer$(LIB_VARIANT).rel

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
