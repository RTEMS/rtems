##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libstackchk-tmp
LIB = ${ARCH}/${LIBNAME}.a

C_FILES = check.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = internal.h
INSTALLED_H_FILES = stackchk.h
noinst_HEADERS = $(H_FILES) $(INSTALLED_H_FILES)

SRCS = $(C_FILES) $(H_FILES) $(INSTALLED_H_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/lib.cfg

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems \
$(INSTALLED_H_FILES:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

${LIB}: ${OBJS}
	$(make-library)

all: ${ARCH} $(PREINSTALL_FILES) $(LIB) $(TMPINSTALL_FILES)

EXTRA_DIST += $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
