##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

include_rtemsdir = $(includedir)/rtems

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

include_rtems_HEADERS = \
    include/rtems/bdbuf.h include/rtems/blkdev.h \
    include/rtems/diskdevs.h include/rtems/ramdisk.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

LIB = ${ARCH}/libblock.a

C_FILES = src/bdbuf.c src/blkdev.c src/diskdevs.c src/ramdisk.c
OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

AM_CFLAGS += $(LIBC_DEFINES)

${ARCH}/%.$(OBJEXT): src/%.c
	${COMPILE} -o $@ -c $<

$(LIB): ${OBJS}
	$(make-library)

all-local: $(PREINSTALL_FILES) ${ARCH} $(LIB)

include $(top_srcdir)/../automake/local.am
