##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libmw-fb-tmp
LIB = ${ARCH}/${LIBNAME}.a

C_FILES = mw_fb.c mw_uid.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = mw_fb.h mw_uid.h
noinst_HEADERS = $(H_FILES)

SRCS = $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rtems/%)

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

EXTRA_DIST = $(C_FILES) $(H_FILES)

include $(top_srcdir)/../../../automake/local.am
