#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

PGMNAME = dummy
PGM = ${ARCH}/$(PGMNAME).rel

# C source names, if any, go here -- minus the .c
C_FILES = dummy.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES =

SRCS = $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/lib.cfg

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems \
$(PROJECT_RELEASE)/lib/$(PGMNAME)$(LIB_VARIANT).rel

$(PROJECT_INCLUDE)/rtems):
	@$(mkinstalldirs) $@

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: ${SRCS} ${OBJS}
	$(make-rel)

$(PROJECT_RELEASE)/lib/$(PGMNAME)$(LIB_VARIANT).rel: $(PGM)
	$(INSTALL_DATA) $< $@

all: ${ARCH} $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
