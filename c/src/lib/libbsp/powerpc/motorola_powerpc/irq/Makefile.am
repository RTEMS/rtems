##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

include_bspdir = $(includedir)/bsp
nodist_include_bsp_HEADERS = $(top_srcdir)/../../powerpc/shared/irq/irq.h

powerpc_shared_irq_C_FILES = \
    $(top_srcdir)/../../powerpc/shared/irq/irq_init.c \
    $(top_srcdir)/../../powerpc/shared/irq/i8259.c \
    $(top_srcdir)/../../powerpc/shared/irq/irq.c
OBJS = \
    $(powerpc_shared_irq_C_FILES:$(top_srcdir)/../../powerpc/shared/irq/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/irq/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

powerpc_shared_irq_S_FILES = \
    $(top_srcdir)/../../powerpc/shared/irq/irq_asm.S
OBJS += \
    $(powerpc_shared_irq_S_FILES:$(top_srcdir)/../../powerpc/shared/irq/%.S=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/irq/%.S $(ARCH)/$(dirstamp)
	${CCASCOMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/irq.h: $(top_srcdir)/../../powerpc/shared/irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

include $(top_srcdir)/../../../../automake/local.am
