##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

sapi_C_FILES = sapi/no-ext.c sapi/no-io.c
C_O_FILES = $(sapi_C_FILES:sapi/%.c=$(ARCH)/%.$(OBJEXT))

rtems_C_FILES = rtems/no-dpmem.c rtems/no-event.c rtems/no-msg.c rtems/no-mp.c \
    rtems/no-part.c rtems/no-region.c \
    rtems/no-rtmon.c rtems/no-sem.c rtems/no-signal.c rtems/no-timer.c
C_O_FILES += $(rtems_C_FILES:rtems/%.c=${ARCH}/%.$(OBJEXT))

PGMS = $(rtems_C_FILES:rtems/%.c=$(ARCH)/%$(LIB_VARIANT).rel)
PGMS += $(sapi_C_FILES:sapi/%.c=$(ARCH)/%$(LIB_VARIANT).rel)

${ARCH}/%.$(OBJEXT): sapi/%.c
	test -d $(ARCH) || mkdir $(ARCH)
	${COMPILE} -o $@ -c $<

${ARCH}/%.$(OBJEXT): rtems/%.c
	test -d $(ARCH) || mkdir $(ARCH)
	${COMPILE} -o $@ -c $<

# Make foo.rel from foo.o
${ARCH}/%$(LIB_VARIANT).rel: ${ARCH}/%.o
	test -d $(ARCH) || mkdir $(ARCH)
	${make-rel}

bsplib_DATA = $(PGMS)

TMPINSTALL_FILES += \
    $(PGMS:${ARCH}/%$(LIB_VARIANT).rel=$(PROJECT_RELEASE)/lib/%$(LIB_VARIANT).rel)

$(PROJECT_RELEASE)/lib/%$(LIB_VARIANT).rel: $(ARCH)/%$(LIB_VARIANT).rel
	$(INSTALL_DATA) $< $@

all-local: ${ARCH} $(PGMS) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGMS) $(C_O_FILES)

EXTRA_DIST = $(rtems_C_FILES) $(sapi_C_FILES)

include $(top_srcdir)/../../../automake/local.am
