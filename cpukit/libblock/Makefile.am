##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/bdbuf.h include/rtems/blkdev.h \
    include/rtems/diskdevs.h include/rtems/ramdisk.h \
    include/rtems/ide_part_table.h

LIB = ${ARCH}/libblock.a

C_FILES = src/bdbuf.c src/blkdev.c src/diskdevs.c src/ramdisk.c \
    src/ide_part_table.c

OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

AM_CPPFLAGS += -I$(top_builddir)
AM_CPPFLAGS += $(LIBC_DEFINES)

${ARCH}/%.$(OBJEXT): src/%.c
	${COMPILE} -o $@ -c $<

$(LIB): ${OBJS}
	$(make-library)

all-local: $(PREINSTALL_FILES) ${ARCH} $(LIB)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems

$(PROJECT_INCLUDE)/rtems/bdbuf.h: include/rtems/bdbuf.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/bdbuf.h

$(PROJECT_INCLUDE)/rtems/blkdev.h: include/rtems/blkdev.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/blkdev.h

$(PROJECT_INCLUDE)/rtems/diskdevs.h: include/rtems/diskdevs.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/diskdevs.h

$(PROJECT_INCLUDE)/rtems/ramdisk.h: include/rtems/ramdisk.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ramdisk.h

$(PROJECT_INCLUDE)/rtems/ide_part_table.h: include/rtems/ide_part_table.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ide_part_table.h

include $(top_srcdir)/automake/local.am
