##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/shmdr.rel

C_FILES = addlq.c cnvpkt.c getlq.c dump.c fatal.c getpkt.c init.c initlq.c \
    intr.c mpisr.c poll.c receive.c retpkt.c send.c setckvec.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = shm_driver.h mpci.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

$(PROJECT_RELEASE)/lib/shmdr$(LIB_VARIANT).rel: $(ARCH)/shmdr.rel
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by
#   ../$(RTEMS_BSP_FAMILY)/wrapup/Makefile

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/shm_driver.h: shm_driver.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpci.h: mpci.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/shm_driver.h \
    $(PROJECT_INCLUDE)/mpci.h

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/shmdr$(LIB_VARIANT).rel

if HAS_MP
all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(PGM) $(TMPINSTALL_FILES)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = README addlq.c cnvpkt.c dump.c fatal.c getlq.c getpkt.c init.c \
    initlq.c intr.c mpci.h mpisr.c poll.c receive.c retpkt.c send.c \
    setckvec.c shm_driver.h

include $(top_srcdir)/../../../../automake/local.am
