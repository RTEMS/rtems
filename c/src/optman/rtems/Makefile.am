##
##  $Id$
##


C_FILES = no-dpmem.c no-event.c no-msg.c no-mp.c no-part.c no-region.c \
    no-rtmon.c no-sem.c no-signal.c no-timer.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

PGMS = $(C_FILES:%.c=$(ARCH)/%$(LIB_VARIANT).rel)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am
include ../rel.am

include $(top_srcdir)/../../../automake/local.am
