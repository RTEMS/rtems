## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

SUBDIRS = rtems

# C source names
C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = asm.h i960RP.h
noinst_HEADERS = $(H_FILES)

# S source names
S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE) \
$(H_FILES:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += \
$(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o

$(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
	$(INSTALL_DATA) $< $@

REL = $(ARCH)/rtems-cpu.rel

$(REL): $(C_O_FILES) $(S_O_FILES)
	$(make-rel)

all-local: $(ARCH) $(ARCH)/rtems.o $(REL) $(TMPINSTALL_FILES)

EXTRA_DIST = \
cpu.c \
cpu_asm.S \
rtems.S

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
