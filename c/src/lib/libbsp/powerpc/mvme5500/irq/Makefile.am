##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/irq
INCLUDES = -I @srcdir@/../irq

C_FILES = irq_init.c GT64260Int.c irq.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS =  irq.h

H_FILES = irq.h

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = irq_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

EXTRA_DIST = irq_init.c GT64260Int.c irq.c

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $<

$(PROJECT_INCLUDE)/bsp/irq.h: irq.h
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
