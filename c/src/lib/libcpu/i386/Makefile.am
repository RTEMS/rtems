## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

LIBNAME = libcpu
LIB = $(ARCH)/$(LIBNAME).a

# C source names, if any, go here -- minus the .c
C_FILES = cpu.c displayCpu.c page.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = cpu.h registers.h cpuModel.h
noinst_HEADERS = $(H_FILES)

# Assembly source names, if any, go here -- minus the .s
S_FILES = cpu_asm.S cpuModel.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

$(LIB): ${OBJS}
	$(make-library)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(H_FILES:%=$(PROJECT_INCLUDE)/libcpu/%)

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += \
$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@


all-local: ${ARCH} $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
