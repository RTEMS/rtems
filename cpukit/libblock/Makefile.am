##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

if !UNIX
noinst_LIBRARIES = libblock.a
libblock_a_SOURCES = src/bdbuf.c src/blkdev.c src/diskdevs.c src/flashdisk.c \
    src/ramdisk.c src/ide_part_table.c src/show_bdbuf.c src/nvdisk.c \
    src/nvdisk-sram.c \
    include/rtems/bdbuf.h include/rtems/blkdev.h \
    include/rtems/diskdevs.h include/rtems/flashdisk.h \
    include/rtems/ramdisk.h include/rtems/nvdisk.h include/rtems/nvdisk-sram.h \
    include/rtems/ide_part_table.h
endif

include $(top_srcdir)/automake/local.am
