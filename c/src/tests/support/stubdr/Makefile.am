##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/stubdr.rel

C_FILES = open.c close.c read.c write.c init.c cntrl.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = stubdrv.h
noinst_HEADERS = $(H_FILES)

SRCS = $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/leaf.am

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): ${OBJS}
	$(make-rel)

all-local: ${ARCH} $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../automake/local.am
