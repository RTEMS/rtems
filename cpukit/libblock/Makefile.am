##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/amcompile.am

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/bdbuf.h include/rtems/blkdev.h \
    include/rtems/diskdevs.h include/rtems/ramdisk.h \
    include/rtems/ide_part_table.h

C_FILES = src/bdbuf.c src/blkdev.c src/diskdevs.c src/ramdisk.c \
    src/ide_part_table.c

EXTRA_LIBRARIES = o-optimize/libblock.a
o_optimize_libblock_a_SOURCES = $(C_FILES)
o_optimize_libblock_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += o-debug/libblock.a
o_debug_libblock_a_SOURCES = $(C_FILES)
o_debug_libblock_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

AM_CPPFLAGS += -I$(top_builddir)
AM_CPPFLAGS += $(LIBC_DEFINES)

all-local: $(PREINSTALL_FILES) $(ARCH)/libblock.a

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/bdbuf.h: include/rtems/bdbuf.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/bdbuf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/bdbuf.h

$(PROJECT_INCLUDE)/rtems/blkdev.h: include/rtems/blkdev.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/blkdev.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/blkdev.h

$(PROJECT_INCLUDE)/rtems/diskdevs.h: include/rtems/diskdevs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/diskdevs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/diskdevs.h

$(PROJECT_INCLUDE)/rtems/ramdisk.h: include/rtems/ramdisk.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/ramdisk.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ramdisk.h

$(PROJECT_INCLUDE)/rtems/ide_part_table.h: include/rtems/ide_part_table.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/ide_part_table.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ide_part_table.h

include $(top_srcdir)/automake/local.am
