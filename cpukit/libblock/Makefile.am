include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libblock.a
libblock_a_SOURCES = src/bdbuf.c \
    src/blkdev.c \
    src/blkdev-imfs.c \
    src/blkdev-ioctl.c \
    src/blkdev-ops.c \
    src/blkdev-print-stats.c \
    src/blkdev-blkstats.c \
    src/diskdevs.c \
    src/diskdevs-init.c \
    src/flashdisk.c \
    src/ramdisk-driver.c \
    src/ramdisk-init.c \
    src/ramdisk-config.c \
    src/ramdisk-register.c \
    src/ide_part_table.c \
    src/nvdisk.c \
    src/nvdisk-sram.c \
    src/bdpart-create.c \
    src/bdpart-dump.c \
    src/bdpart-mount.c \
    src/bdpart-read.c \
    src/bdpart-register.c \
    src/bdpart-sort.c \
    src/bdpart-write.c \
    src/media-path.c \
    src/media.c \
    src/media-server.c \
    src/media-desc.c \
    src/media-dev-ident.c \
    include/rtems/bdbuf.h include/rtems/blkdev.h \
    include/rtems/diskdevs.h include/rtems/flashdisk.h \
    include/rtems/ramdisk.h include/rtems/nvdisk.h include/rtems/nvdisk-sram.h \
    include/rtems/ide_part_table.h

include $(top_srcdir)/automake/local.am
