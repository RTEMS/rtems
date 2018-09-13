ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

project_lib_LIBRARIES =
TMPINSTALL_FILES =

noinst_LIBRARIES = libcpukit.a

libcpukit_a_SOURCES =
libcpukit_a_SOURCES += dev/i2c/eeprom.c
libcpukit_a_SOURCES += dev/i2c/fpga-i2c-slave.c
libcpukit_a_SOURCES += dev/i2c/gpio-nxp-pca9535.c
libcpukit_a_SOURCES += dev/i2c/i2c-bus.c
libcpukit_a_SOURCES += dev/i2c/i2c-dev.c
libcpukit_a_SOURCES += dev/i2c/sensor-lm75a.c
libcpukit_a_SOURCES += dev/i2c/switch-nxp-pca9548a.c
libcpukit_a_SOURCES += dev/i2c/ti-ads-16bit-adc.c
libcpukit_a_SOURCES += dev/i2c/ti-lm25066a.c
libcpukit_a_SOURCES += dev/i2c/ti-tmp112.c
libcpukit_a_SOURCES += dev/i2c/xilinx-axi-i2c.c
libcpukit_a_SOURCES += dev/serial/sc16is752.c
libcpukit_a_SOURCES += dev/serial/sc16is752-spi.c
libcpukit_a_SOURCES += dev/spi/spi-bus.c
libcpukit_a_SOURCES += dtc/libfdt/fdt_addresses.c
libcpukit_a_SOURCES += dtc/libfdt/fdt.c
libcpukit_a_SOURCES += dtc/libfdt/fdt_empty_tree.c
libcpukit_a_SOURCES += dtc/libfdt/fdt_ro.c
libcpukit_a_SOURCES += dtc/libfdt/fdt_rw.c
libcpukit_a_SOURCES += dtc/libfdt/fdt_strerror.c
libcpukit_a_SOURCES += dtc/libfdt/fdt_sw.c
libcpukit_a_SOURCES += dtc/libfdt/fdt_wip.c
libcpukit_a_SOURCES += libblock/src/bdbuf.c
libcpukit_a_SOURCES += libblock/src/bdpart-create.c
libcpukit_a_SOURCES += libblock/src/bdpart-dump.c
libcpukit_a_SOURCES += libblock/src/bdpart-mount.c
libcpukit_a_SOURCES += libblock/src/bdpart-read.c
libcpukit_a_SOURCES += libblock/src/bdpart-register.c
libcpukit_a_SOURCES += libblock/src/bdpart-sort.c
libcpukit_a_SOURCES += libblock/src/bdpart-write.c
libcpukit_a_SOURCES += libblock/src/blkdev-blkstats.c
libcpukit_a_SOURCES += libblock/src/blkdev.c
libcpukit_a_SOURCES += libblock/src/blkdev-imfs.c
libcpukit_a_SOURCES += libblock/src/blkdev-ioctl.c
libcpukit_a_SOURCES += libblock/src/blkdev-ops.c
libcpukit_a_SOURCES += libblock/src/blkdev-print-stats.c
libcpukit_a_SOURCES += libblock/src/diskdevs.c
libcpukit_a_SOURCES += libblock/src/diskdevs-init.c
libcpukit_a_SOURCES += libblock/src/flashdisk.c
libcpukit_a_SOURCES += libblock/src/ide_part_table.c
libcpukit_a_SOURCES += libblock/src/media.c
libcpukit_a_SOURCES += libblock/src/media-desc.c
libcpukit_a_SOURCES += libblock/src/media-dev-ident.c
libcpukit_a_SOURCES += libblock/src/media-path.c
libcpukit_a_SOURCES += libblock/src/media-server.c
libcpukit_a_SOURCES += libblock/src/nvdisk.c
libcpukit_a_SOURCES += libblock/src/nvdisk-sram.c
libcpukit_a_SOURCES += libblock/src/ramdisk-config.c
libcpukit_a_SOURCES += libblock/src/ramdisk-driver.c
libcpukit_a_SOURCES += libblock/src/ramdisk-init.c
libcpukit_a_SOURCES += libblock/src/ramdisk-register.c
libcpukit_a_SOURCES += libblock/src/sparse-disk.c
libcpukit_a_SOURCES += sapi/src/chainappendnotify.c
libcpukit_a_SOURCES += sapi/src/chaingetnotify.c
libcpukit_a_SOURCES += sapi/src/chaingetwait.c
libcpukit_a_SOURCES += sapi/src/chainprependnotify.c
libcpukit_a_SOURCES += sapi/src/chainprotected.c
libcpukit_a_SOURCES += sapi/src/cpucounterconverter.c
libcpukit_a_SOURCES += sapi/src/delaynano.c
libcpukit_a_SOURCES += sapi/src/delayticks.c
libcpukit_a_SOURCES += sapi/src/exinit.c
libcpukit_a_SOURCES += sapi/src/exshutdown.c
libcpukit_a_SOURCES += sapi/src/extension.c
libcpukit_a_SOURCES += sapi/src/extensioncreate.c
libcpukit_a_SOURCES += sapi/src/extensiondelete.c
libcpukit_a_SOURCES += sapi/src/extensionident.c
libcpukit_a_SOURCES += sapi/src/fatal.c
libcpukit_a_SOURCES += sapi/src/fatalsrctext.c
libcpukit_a_SOURCES += sapi/src/getversionstring.c
libcpukit_a_SOURCES += sapi/src/interrtext.c
libcpukit_a_SOURCES += sapi/src/io.c
libcpukit_a_SOURCES += sapi/src/ioclose.c
libcpukit_a_SOURCES += sapi/src/iocontrol.c
libcpukit_a_SOURCES += sapi/src/ioinitialize.c
libcpukit_a_SOURCES += sapi/src/ioopen.c
libcpukit_a_SOURCES += sapi/src/ioread.c
libcpukit_a_SOURCES += sapi/src/ioregisterdriver.c
libcpukit_a_SOURCES += sapi/src/iounregisterdriver.c
libcpukit_a_SOURCES += sapi/src/iowrite.c
libcpukit_a_SOURCES += sapi/src/panic.c
libcpukit_a_SOURCES += sapi/src/posixapi.c
libcpukit_a_SOURCES += sapi/src/profilingiterate.c
libcpukit_a_SOURCES += sapi/src/profilingreportxml.c
libcpukit_a_SOURCES += sapi/src/rbheap.c
libcpukit_a_SOURCES += sapi/src/rbtree.c
libcpukit_a_SOURCES += sapi/src/rbtreefind.c
libcpukit_a_SOURCES += sapi/src/sapirbtreeinsert.c
libcpukit_a_SOURCES += sapi/src/tcsimpleinstall.c
libcpukit_a_SOURCES += sapi/src/version.c

#
# Create a new Version VC Key header if the VC state has changed.
#
stamp_vc_key = stamp-vc-key

BUILT_SOURCES = version-vc-key.h

.PHONY: generate-vc-key

generate-vc-key:
	@+current_vc_key=""; \
	if test -f $(stamp_vc_key); then \
	 current_vc_key=`cat $(stamp_vc_key)`; \
	fi; \
	vc_key=`$(top_srcdir)/vc-key.sh $(top_srcdir) $$current_vc_key`; \
	if test "$$vc_key" != "matches"; then \
	 echo "Generating version-vc-key.h"; \
	 if test "$$vc_key" == "release"; then \
	  vc_header_key="\/\* No version control key found; release\? \*\/"; \
	 else \
	  vc_header_key="#define RTEMS_VERSION_VC_KEY \"$$vc_key\""; \
	 fi; \
	 cat $(top_srcdir)/version-vc-key.h.in | \
	  sed -e "s/@VERSION_VC_KEY@/$$vc_header_key/g" > version-vc-key.h; \
	 echo "$$vc_key" > $(stamp_vc_key); \
	fi

version-vc-key.h: generate-vc-key

$(top_srcdir)/sapi/src/version.c: version-vc-key.h

all-local: generate-vc-key

# librtemscpu
_SUBDIRS = . score rtems posix
_SUBDIRS += libcrypt
_SUBDIRS += libcsupport libfs
_SUBDIRS += libdrvmgr
_SUBDIRS += libnetworking librpc
_SUBDIRS += libpci
_SUBDIRS += libi2c
_SUBDIRS += libmisc
_SUBDIRS += libmd
_SUBDIRS += libgnat
_SUBDIRS += libdl
_SUBDIRS += libstdthreads
_SUBDIRS += libdebugger
_SUBDIRS += wrapup

# other libraries
_SUBDIRS += zlib
_SUBDIRS += telnetd
_SUBDIRS += pppd
_SUBDIRS += mghttpd

project_lib_LIBRARIES += libftpd.a

$(PROJECT_LIB)/libftpd.a: libftpd.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libftpd.a
TMPINSTALL_FILES += $(PROJECT_LIB)/libftpd.a

libftpd_a_SOURCES =
libftpd_a_SOURCES += ftpd/ftpd.c
libftpd_a_SOURCES += ftpd/ftpd-init.c

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
