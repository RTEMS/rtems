##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/irq

C_FILES = irq_init.c i8259.c irq.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

# FIXME: this does give any sense here
H_FILES = ../../shared/irq/irq.h

S_FILES = irq_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(ARCH) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
