##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/irq

C_FILES = irq_init.c i8259.c irq.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_bspdir = $(includedir)/bsp
nodist_include_bsp_HEADERS = $(top_srcdir)/../shared/irq/irq.h

S_FILES = irq_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(PREINSTALL_FILES) $(OBJS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/irq.h: $(top_srcdir)/../shared/irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

include $(top_srcdir)/../../../../automake/local.am
