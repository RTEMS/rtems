## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/start.o

## Assembly source names
S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_RELEASE)/lib:
	$(mkinstalldirs) $@

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(PGM)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_RELEASE)/lib $(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

all-local: ${ARCH} $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = $(S_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
