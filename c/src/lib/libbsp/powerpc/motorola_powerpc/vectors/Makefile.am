##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# FIXME: this does not make much sense
include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = $(top_srcdir)/../../powerpc/shared/vectors/vectors.h

powerpc_shared_vectors_C_FILES = \
    $(top_srcdir)/../..//powerpc/shared/vectors/vectors_init.c
OBJS = \
    $(powerpc_shared_vectors_C_FILES:$(top_srcdir)/../../powerpc/shared/vectors/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/vectors/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

powerpc_shared_vectors_S_FILES = \
    $(top_srcdir)/../../powerpc/shared/vectors/vectors.S
OBJS += \
    $(powerpc_shared_vectors_S_FILES:$(top_srcdir)/../../powerpc/shared/vectors/%.S=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/vectors/%.S $(ARCH)/$(dirstamp)
	${CCASCOMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/vectors.h: $(top_srcdir)/../../powerpc/shared/vectors/vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h

include $(top_srcdir)/../../../../automake/local.am
