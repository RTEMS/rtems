## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

LIBNAME = libcpu
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = cpu.c displayCpu.c page.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = cpu.h registers.h cpuModel.h

S_FILES = cpu_asm.S cpuModel.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu \
    $(H_FILES:%=$(PROJECT_INCLUDE)/libcpu/%)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

all: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

EXTRA_DIST = cpu.c cpu.h cpuModel.S cpuModel.h cpu_asm.S displayCpu.c page.c \
    registers.h

include $(top_srcdir)/../../../../../automake/local.am
