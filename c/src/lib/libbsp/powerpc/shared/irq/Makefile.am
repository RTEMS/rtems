## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = i8259.c irq.c irq_init.c

H_FILES = irq.h
noinst_HEADERS = $(H_FILES)

S_FILES = irq_asm.S

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/bsp/%.h)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
