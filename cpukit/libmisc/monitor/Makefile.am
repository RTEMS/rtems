##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libmonitor-tmp
LIB = ${ARCH}/$(LIBNAME).a

# C source names, if any, go here -- minus the .c
# We only build multiprocessing related files if HAS_MP was defined
MP_C_FILES = mon-mpci.c

C_FILES = mon-command.c mon-symbols.c mon-prmisc.c mon-monitor.c \
    mon-object.c mon-server.c mon-task.c mon-queue.c mon-driver.c \
    mon-dname.c mon-itask.c mon-extension.c mon-manager.c mon-config.c

if HAS_MP
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o) $(MP_C_FILES:%.c=${ARCH}/%.o)
else
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)
endif

H_FILES = monitor.h symbols.h
noinst_HEADERS = $(H_FILES)

SRCS = README $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/lib.cfg

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems \
$(H_FILES:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

${LIB}: ${OBJS}
	$(make-library)

all: ${ARCH} $(PREINSTALL_FILES) $(LIB) $(TMPINSTALL_FILES)

EXTRA_DIST += $(C_FILES) $(MP_C_FILES)

include $(top_srcdir)/../../../automake/local.am
