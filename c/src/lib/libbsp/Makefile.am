##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

# If we are building a "real" BSP, then we need to descend into the
# appropriate CPU specific directory.  The bare BSP is a special
# case which can be built for any CPU and it resides at the same
# level as the CPUs.  If we are building the bare BSP, then descend
# into that directory.

SUBDIRS = @libbsp_cpu_subdir@
DIST_SUBDIRS = @libbsp_cpu_subdir@

EXTRA_DIST = MERGE.PROCEDURE bsp.am

# shared
EXTRA_DIST += shared/bootcard.c shared/bspclean.c shared/bsplibc.c \
    shared/bsppost.c shared/console-polled.c shared/console.c \
    shared/gnatinstallhandler.c shared/main.c shared/sbrk.c shared/tod.c \
    shared/tod.h
EXTRA_DIST += shared/vmeUniverse/vmeUniverse.c \
    shared/vmeUniverse/vmeUniverse.h
EXTRA_DIST += shared/include/coverhd.h
EXTRA_DIST += shared/gdbstub/rtems-stub-glue.c

# shmdr
shmdr_C_FILES = shmdr/addlq.c shmdr/cnvpkt.c shmdr/getlq.c shmdr/dump.c \
    shmdr/fatal.c shmdr/getpkt.c shmdr/init.c shmdr/initlq.c shmdr/intr.c \
    shmdr/mpisr.c shmdr/poll.c shmdr/receive.c shmdr/retpkt.c shmdr/send.c \
    shmdr/setckvec.c
shmdr_OBJS = $(shmdr_C_FILES:shmdr/%.c=shmdr/$(ARCH)/%.$(OBJEXT))

# the .rel file built here will be put into libbsp.a by
#   ../$(RTEMS_BSP_FAMILY)/wrapup/Makefile

shmdr/$(ARCH)/$(dirstamp):
	@$(mkinstalldirs) shmdr/$(ARCH)
	@: > shmdr/$(ARCH)/$(dirstamp)
shmdr/$(ARCH)/%.$(OBJEXT): shmdr/%.c shmdr/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
shmdr/$(ARCH)/shmdr$(LIB_VARIANT).rel: $(shmdr_OBJS)
	$(make-rel)

if HAS_MP
include_HEADERS = shmdr/shm_driver.h shmdr/mpci.h

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)
project_lib_DATA = shmdr/$(ARCH)/shmdr$(LIB_VARIANT).rel
endif

EXTRA_DIST += shmdr/README shmdr/addlq.c shmdr/cnvpkt.c shmdr/dump.c \
    shmdr/fatal.c shmdr/getlq.c shmdr/getpkt.c shmdr/init.c shmdr/initlq.c \
    shmdr/intr.c shmdr/mpisr.c shmdr/poll.c shmdr/receive.c shmdr/retpkt.c \
    shmdr/send.c shmdr/setckvec.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_MP
$(PROJECT_INCLUDE)/shm_driver.h: shmdr/shm_driver.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/shm_driver.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/shm_driver.h

$(PROJECT_INCLUDE)/mpci.h: shmdr/mpci.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpci.h

${PROJECT_RELEASE}/lib/shmdr$(LIB_VARIANT).rel: shmdr/$(ARCH)/shmdr$(LIB_VARIANT).rel ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/shmdr$(LIB_VARIANT).rel
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/shmdr$(LIB_VARIANT).rel
endif

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
