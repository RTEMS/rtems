##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -I$(top_builddir)

include_rtemsdir = $(includedir)/rtems

EXTRA_LIBRARIES = libblock.a
CLEANFILES = libblock.a
libblock_a_CPPFLAGS = $(AM_CPPFLAGS) $(LIBC_DEFINES) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libblock_g.a
CLEANFILES += libblock_g.a
libblock_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(LIBC_DEFINES) $(CFLAGS_DEBUG_V)

include_rtems_HEADERS = include/rtems/bdbuf.h include/rtems/blkdev.h \
    include/rtems/diskdevs.h include/rtems/ramdisk.h \
    include/rtems/ide_part_table.h

libblock_a_SOURCES = src/bdbuf.c src/blkdev.c src/diskdevs.c src/ramdisk.c \
    src/ide_part_table.c

libblock_g_a_SOURCES = $(libblock_a_SOURCES)

all-local: $(PREINSTALL_FILES) libblock$(LIB_VARIANT).a

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

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

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
