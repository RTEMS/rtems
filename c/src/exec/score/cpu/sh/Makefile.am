## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

SUBDIRS = rtems

H_FILES = asm.h
noinst_HEADERS = $(H_FILES)

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

# C source names, if any, go here -- minus the .c
C_FILES = cpu.c cpu_asm.c isp$(RTEMS_CPU_MODEL).c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

REL = $(ARCH)/rtems-cpu.rel

$(REL): $(C_O_FILES)
	$(make-rel)

all: $(ARCH) $(ARCH)/rtems.o $(REL) $(TMPINSTALL_FILES)

EXTRA_DIST = \
cpu.c \
cpu_asm.c \
ispsh7032.c \
ispsh7045.c \
rtems.c

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
