##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../shared/irq

PGM = $(ARCH)/irq.rel

C_FILES = irq.c irq_init.c bsp_irq_init.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

S_FILES = irq_asm.S bsp_irq_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

H_FILES = irq.h

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -I$(srcdir)

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = bsp_irq_asm.S bsp_irq_init.c irq.c irq.h

include $(top_srcdir)/../../../../../../automake/local.am
