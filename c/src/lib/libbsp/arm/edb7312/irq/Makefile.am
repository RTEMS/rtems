##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/irq

PGM = $(ARCH)/irq.rel

C_FILES = irq.c bsp_irq_init.c irq_init.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

S_FILES = bsp_irq_asm.S irq_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

include_HEADERS = irq.h

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -I$(srcdir)

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = bsp_irq_asm.S bsp_irq_init.c irq.c

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/irq.h: irq.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/irq.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../../automake/local.am
