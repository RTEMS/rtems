##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtemsdir = $(includedir)/rtems

LIBNAME = libmw-fb-tmp
LIB = ${ARCH}/${LIBNAME}.a

C_FILES = mw_fb.c mw_uid.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

include_rtems_HEADERS = mw_fb.h mw_uid.h

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

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

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
