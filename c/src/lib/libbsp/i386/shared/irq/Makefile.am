## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = irq.h irq_asm.h

C_FILES = irq.c irq_init.c
S_FILES = irq_asm.S

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all: $(PREINSTALL_FILES)

EXTRA_DIST = irq.c irq.h irq_asm.S irq_asm.h irq_init.c

include $(top_srcdir)/../../../../../automake/local.am
