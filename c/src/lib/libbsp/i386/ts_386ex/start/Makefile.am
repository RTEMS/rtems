##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGMS = ${ARCH}/start.o

# Assembly source names, if any, go here -- minus the .S
S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

H_FILES = 80386ex.h
noinst_HEADERS = $(H_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

if RTEMS_GAS_CODE16
AM_CPPFLAGS += -DNEW_GAS
endif

$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o: $(ARCH)/start.o
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += \
$(PROJECT_RELEASE)/lib/start$(LIB_VARIANT).o

all-local: $(ARCH) $(TMPINSTALL_FILES)

EXTRA_DIST = 80386ex.inc macros.inc ts_1325.inc start.S

include $(top_srcdir)/../../../../../../automake/local.am
