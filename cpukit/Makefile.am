ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am
include $(srcdir)/header-dirs.am
include $(srcdir)/headers.am

if LIBNETWORKING

include $(srcdir)/libnetworking/headers.am

endif

# This file is in the build tree
include_rtems_score_HEADERS += ../include/rtems/score/cpuopts.h

project_lib_LIBRARIES =

project_lib_LIBRARIES += librtemscpu.a

librtemscpu_a_SOURCES =
librtemscpu_a_SOURCES += dev/i2c/eeprom.c
librtemscpu_a_SOURCES += dev/i2c/fpga-i2c-slave.c
librtemscpu_a_SOURCES += dev/i2c/gpio-nxp-pca9535.c
librtemscpu_a_SOURCES += dev/i2c/i2c-bus.c
librtemscpu_a_SOURCES += dev/i2c/i2c-dev.c
librtemscpu_a_SOURCES += dev/i2c/sensor-lm75a.c
librtemscpu_a_SOURCES += dev/i2c/switch-nxp-pca9548a.c
librtemscpu_a_SOURCES += dev/i2c/ti-ads-16bit-adc.c
librtemscpu_a_SOURCES += dev/i2c/ti-lm25066a.c
librtemscpu_a_SOURCES += dev/i2c/ti-tmp112.c
librtemscpu_a_SOURCES += dev/i2c/xilinx-axi-i2c.c
librtemscpu_a_SOURCES += dev/serial/sc16is752.c
librtemscpu_a_SOURCES += dev/serial/sc16is752-spi.c
librtemscpu_a_SOURCES += dev/spi/spi-bus.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt_addresses.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt_empty_tree.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt_ro.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt_rw.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt_strerror.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt_sw.c
librtemscpu_a_SOURCES += dtc/libfdt/fdt_wip.c
librtemscpu_a_SOURCES += libblock/src/bdbuf.c
librtemscpu_a_SOURCES += libblock/src/bdpart-create.c
librtemscpu_a_SOURCES += libblock/src/bdpart-dump.c
librtemscpu_a_SOURCES += libblock/src/bdpart-mount.c
librtemscpu_a_SOURCES += libblock/src/bdpart-read.c
librtemscpu_a_SOURCES += libblock/src/bdpart-register.c
librtemscpu_a_SOURCES += libblock/src/bdpart-sort.c
librtemscpu_a_SOURCES += libblock/src/bdpart-write.c
librtemscpu_a_SOURCES += libblock/src/blkdev-blkstats.c
librtemscpu_a_SOURCES += libblock/src/blkdev.c
librtemscpu_a_SOURCES += libblock/src/blkdev-imfs.c
librtemscpu_a_SOURCES += libblock/src/blkdev-ioctl.c
librtemscpu_a_SOURCES += libblock/src/blkdev-ops.c
librtemscpu_a_SOURCES += libblock/src/blkdev-print-stats.c
librtemscpu_a_SOURCES += libblock/src/diskdevs.c
librtemscpu_a_SOURCES += libblock/src/diskdevs-init.c
librtemscpu_a_SOURCES += libblock/src/flashdisk.c
librtemscpu_a_SOURCES += libblock/src/ide_part_table.c
librtemscpu_a_SOURCES += libblock/src/media.c
librtemscpu_a_SOURCES += libblock/src/media-desc.c
librtemscpu_a_SOURCES += libblock/src/media-dev-ident.c
librtemscpu_a_SOURCES += libblock/src/media-path.c
librtemscpu_a_SOURCES += libblock/src/media-server.c
librtemscpu_a_SOURCES += libblock/src/nvdisk.c
librtemscpu_a_SOURCES += libblock/src/nvdisk-sram.c
librtemscpu_a_SOURCES += libblock/src/ramdisk-config.c
librtemscpu_a_SOURCES += libblock/src/ramdisk-driver.c
librtemscpu_a_SOURCES += libblock/src/ramdisk-init.c
librtemscpu_a_SOURCES += libblock/src/ramdisk-register.c
librtemscpu_a_SOURCES += libblock/src/sparse-disk.c
librtemscpu_a_SOURCES += libcrypt/crypt.c
librtemscpu_a_SOURCES += libcrypt/crypt-md5.c
librtemscpu_a_SOURCES += libcrypt/crypt-sha256.c
librtemscpu_a_SOURCES += libcrypt/crypt-sha512.c
librtemscpu_a_SOURCES += libcrypt/misc.c
librtemscpu_a_SOURCES += libcsupport/src/access.c
librtemscpu_a_SOURCES += libcsupport/src/arc4random_getentropy_fail.c
librtemscpu_a_SOURCES += libcsupport/src/alignedalloc.c
librtemscpu_a_SOURCES += libcsupport/src/__assert.c
librtemscpu_a_SOURCES += libcsupport/src/assoc32tostring.c
librtemscpu_a_SOURCES += libcsupport/src/assoclocalbyname.c
librtemscpu_a_SOURCES += libcsupport/src/assoclocalbyremotebitfield.c
librtemscpu_a_SOURCES += libcsupport/src/assoclocalbyremote.c
librtemscpu_a_SOURCES += libcsupport/src/assocnamebad.c
librtemscpu_a_SOURCES += libcsupport/src/assocnamebylocalbitfield.c
librtemscpu_a_SOURCES += libcsupport/src/assocnamebylocal.c
librtemscpu_a_SOURCES += libcsupport/src/assocnamebyremotebitfield.c
librtemscpu_a_SOURCES += libcsupport/src/assocnamebyremote.c
librtemscpu_a_SOURCES += libcsupport/src/assocptrbylocal.c
librtemscpu_a_SOURCES += libcsupport/src/assocptrbyname.c
librtemscpu_a_SOURCES += libcsupport/src/assocptrbyremote.c
librtemscpu_a_SOURCES += libcsupport/src/assocremotebylocalbitfield.c
librtemscpu_a_SOURCES += libcsupport/src/assocremotebylocal.c
librtemscpu_a_SOURCES += libcsupport/src/assocremotebyname.c
librtemscpu_a_SOURCES += libcsupport/src/assocthreadstatestostring.c
librtemscpu_a_SOURCES += libcsupport/src/base_fs.c
librtemscpu_a_SOURCES += libcsupport/src/cachealignedalloc.c
librtemscpu_a_SOURCES += libcsupport/src/cachecoherentalloc.c
librtemscpu_a_SOURCES += libcsupport/src/calloc.c
librtemscpu_a_SOURCES += libcsupport/src/_calloc_r.c
librtemscpu_a_SOURCES += libcsupport/src/cfgetispeed.c
librtemscpu_a_SOURCES += libcsupport/src/cfgetospeed.c
librtemscpu_a_SOURCES += libcsupport/src/cfmakeraw.c
librtemscpu_a_SOURCES += libcsupport/src/cfmakesane.c
librtemscpu_a_SOURCES += libcsupport/src/cfsetispeed.c
librtemscpu_a_SOURCES += libcsupport/src/cfsetospeed.c
librtemscpu_a_SOURCES += libcsupport/src/cfsetspeed.c
librtemscpu_a_SOURCES += libcsupport/src/chdir.c
librtemscpu_a_SOURCES += libcsupport/src/chmod.c
librtemscpu_a_SOURCES += libcsupport/src/chown.c
librtemscpu_a_SOURCES += libcsupport/src/chroot.c
librtemscpu_a_SOURCES += libcsupport/src/clock.c
librtemscpu_a_SOURCES += libcsupport/src/clonenode.c
librtemscpu_a_SOURCES += libcsupport/src/close.c
librtemscpu_a_SOURCES += libcsupport/src/consolesimple.c
librtemscpu_a_SOURCES += libcsupport/src/consolesimpleread.c
librtemscpu_a_SOURCES += libcsupport/src/consolesimpletask.c
librtemscpu_a_SOURCES += libcsupport/src/ctermid.c
librtemscpu_a_SOURCES += libcsupport/src/dup2.c
librtemscpu_a_SOURCES += libcsupport/src/dup.c
librtemscpu_a_SOURCES += libcsupport/src/error.c
librtemscpu_a_SOURCES += libcsupport/src/fchdir.c
librtemscpu_a_SOURCES += libcsupport/src/fchmod.c
librtemscpu_a_SOURCES += libcsupport/src/fchown.c
librtemscpu_a_SOURCES += libcsupport/src/fcntl.c
librtemscpu_a_SOURCES += libcsupport/src/fdatasync.c
librtemscpu_a_SOURCES += libcsupport/src/flockfile.c
librtemscpu_a_SOURCES += libcsupport/src/fpathconf.c
librtemscpu_a_SOURCES += libcsupport/src/free.c
librtemscpu_a_SOURCES += libcsupport/src/freenode.c
librtemscpu_a_SOURCES += libcsupport/src/_free_r.c
librtemscpu_a_SOURCES += libcsupport/src/fstat.c
librtemscpu_a_SOURCES += libcsupport/src/fsync.c
librtemscpu_a_SOURCES += libcsupport/src/ftruncate.c
librtemscpu_a_SOURCES += libcsupport/src/ftrylockfile.c
librtemscpu_a_SOURCES += libcsupport/src/funlockfile.c
librtemscpu_a_SOURCES += libcsupport/src/getchark.c
librtemscpu_a_SOURCES += libcsupport/src/getdents.c
librtemscpu_a_SOURCES += libcsupport/src/getegid.c
librtemscpu_a_SOURCES += libcsupport/src/geteuid.c
librtemscpu_a_SOURCES += libcsupport/src/getgid.c
librtemscpu_a_SOURCES += libcsupport/src/getgrent.c
librtemscpu_a_SOURCES += libcsupport/src/getgrnam.c
librtemscpu_a_SOURCES += libcsupport/src/getgroups.c
librtemscpu_a_SOURCES += libcsupport/src/getlogin.c
librtemscpu_a_SOURCES += libcsupport/src/getpagesize.c
librtemscpu_a_SOURCES += libcsupport/src/getpgrp.c
librtemscpu_a_SOURCES += libcsupport/src/__getpid.c
librtemscpu_a_SOURCES += libcsupport/src/getpid.c
librtemscpu_a_SOURCES += libcsupport/src/getppid.c
librtemscpu_a_SOURCES += libcsupport/src/getpwent.c
librtemscpu_a_SOURCES += libcsupport/src/getreentglobal.c
librtemscpu_a_SOURCES += libcsupport/src/getrusage.c
librtemscpu_a_SOURCES += libcsupport/src/__gettod.c
librtemscpu_a_SOURCES += libcsupport/src/getuid.c
librtemscpu_a_SOURCES += libcsupport/src/gxx_wrappers.c
librtemscpu_a_SOURCES += libcsupport/src/ioctl.c
librtemscpu_a_SOURCES += libcsupport/src/isatty_r.c
librtemscpu_a_SOURCES += libcsupport/src/issetugid.c
librtemscpu_a_SOURCES += libcsupport/src/kill_noposix.c
librtemscpu_a_SOURCES += libcsupport/src/lchown.c
librtemscpu_a_SOURCES += libcsupport/src/libio.c
librtemscpu_a_SOURCES += libcsupport/src/libio_exit.c
librtemscpu_a_SOURCES += libcsupport/src/libio_init.c
librtemscpu_a_SOURCES += libcsupport/src/link.c
librtemscpu_a_SOURCES += libcsupport/src/lseek.c
librtemscpu_a_SOURCES += libcsupport/src/lstat.c
librtemscpu_a_SOURCES += libcsupport/src/malloc.c
librtemscpu_a_SOURCES += libcsupport/src/malloc_deferred.c
librtemscpu_a_SOURCES += libcsupport/src/malloc_dirtier.c
librtemscpu_a_SOURCES += libcsupport/src/mallocdirtydefault.c
librtemscpu_a_SOURCES += libcsupport/src/mallocextenddefault.c
librtemscpu_a_SOURCES += libcsupport/src/mallocfreespace.c
librtemscpu_a_SOURCES += libcsupport/src/mallocgetheapptr.c
librtemscpu_a_SOURCES += libcsupport/src/mallocinfo.c
librtemscpu_a_SOURCES += libcsupport/src/malloc_initialize.c
librtemscpu_a_SOURCES += libcsupport/src/_malloc_r.c
librtemscpu_a_SOURCES += libcsupport/src/mallocsetheapptr.c
librtemscpu_a_SOURCES += libcsupport/src/malloc_walk.c
librtemscpu_a_SOURCES += libcsupport/src/mkdir.c
librtemscpu_a_SOURCES += libcsupport/src/mkfifo.c
librtemscpu_a_SOURCES += libcsupport/src/mknod.c
librtemscpu_a_SOURCES += libcsupport/src/mount.c
librtemscpu_a_SOURCES += libcsupport/src/mount-mgr.c
librtemscpu_a_SOURCES += libcsupport/src/mount-mktgt.c
librtemscpu_a_SOURCES += libcsupport/src/newlibc_exit.c
librtemscpu_a_SOURCES += libcsupport/src/newlibc_reent.c
librtemscpu_a_SOURCES += libcsupport/src/open.c
librtemscpu_a_SOURCES += libcsupport/src/open_dev_console.c
librtemscpu_a_SOURCES += libcsupport/src/pathconf.c
librtemscpu_a_SOURCES += libcsupport/src/posix_devctl.c
librtemscpu_a_SOURCES += libcsupport/src/posix_memalign.c
librtemscpu_a_SOURCES += libcsupport/src/printerfprintfputc.c
librtemscpu_a_SOURCES += libcsupport/src/printertask.c
librtemscpu_a_SOURCES += libcsupport/src/printf_plugin.c
librtemscpu_a_SOURCES += libcsupport/src/print_fprintf.c
librtemscpu_a_SOURCES += libcsupport/src/printk.c
librtemscpu_a_SOURCES += libcsupport/src/printk_plugin.c
librtemscpu_a_SOURCES += libcsupport/src/print_printf.c
librtemscpu_a_SOURCES += libcsupport/src/privateenv.c
librtemscpu_a_SOURCES += libcsupport/src/putk.c
librtemscpu_a_SOURCES += libcsupport/src/pwdgrp.c
librtemscpu_a_SOURCES += libcsupport/src/read.c
librtemscpu_a_SOURCES += libcsupport/src/readlink.c
librtemscpu_a_SOURCES += libcsupport/src/readv.c
librtemscpu_a_SOURCES += libcsupport/src/realloc.c
librtemscpu_a_SOURCES += libcsupport/src/_realloc_r.c
librtemscpu_a_SOURCES += libcsupport/src/realpath.c
librtemscpu_a_SOURCES += libcsupport/src/_rename_r.c
librtemscpu_a_SOURCES += libcsupport/src/resource_snapshot.c
librtemscpu_a_SOURCES += libcsupport/src/rmdir.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_heap_extend.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_heap_extend_via_sbrk.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_heap_greedy.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_heap_null_extend.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_memalign.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_mkdir.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_putc.c
librtemscpu_a_SOURCES += libcsupport/src/rtems_put_char.c
librtemscpu_a_SOURCES += libcsupport/src/setegid.c
librtemscpu_a_SOURCES += libcsupport/src/seteuid.c
librtemscpu_a_SOURCES += libcsupport/src/setgid.c
librtemscpu_a_SOURCES += libcsupport/src/setgroups.c
librtemscpu_a_SOURCES += libcsupport/src/setpgid.c
librtemscpu_a_SOURCES += libcsupport/src/setsid.c
librtemscpu_a_SOURCES += libcsupport/src/setuid.c
librtemscpu_a_SOURCES += libcsupport/src/stat.c
librtemscpu_a_SOURCES += libcsupport/src/statvfs.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_check_permissions.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_deviceio.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_eval_path.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_eval_path_generic.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_exist_in_same_instance.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_location.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_mount_iterate.c
librtemscpu_a_SOURCES += libcsupport/src/sup_fs_next_token.c
librtemscpu_a_SOURCES += libcsupport/src/symlink.c
librtemscpu_a_SOURCES += libcsupport/src/sync.c
librtemscpu_a_SOURCES += libcsupport/src/tcdrain.c
librtemscpu_a_SOURCES += libcsupport/src/tcflow.c
librtemscpu_a_SOURCES += libcsupport/src/tcflush.c
librtemscpu_a_SOURCES += libcsupport/src/tcgetattr.c
librtemscpu_a_SOURCES += libcsupport/src/tcgetpgrp.c
librtemscpu_a_SOURCES += libcsupport/src/tcsendbreak.c
librtemscpu_a_SOURCES += libcsupport/src/tcsetattr.c
librtemscpu_a_SOURCES += libcsupport/src/tcsetpgrp.c
librtemscpu_a_SOURCES += libcsupport/src/termios_baud2index.c
librtemscpu_a_SOURCES += libcsupport/src/termios_baud2num.c
librtemscpu_a_SOURCES += libcsupport/src/termios_baudtable.c
librtemscpu_a_SOURCES += libcsupport/src/termios.c
librtemscpu_a_SOURCES += libcsupport/src/termiosinitialize.c
librtemscpu_a_SOURCES += libcsupport/src/termios_num2baud.c
librtemscpu_a_SOURCES += libcsupport/src/termios_posix_isig_handler.c
librtemscpu_a_SOURCES += libcsupport/src/termios_setbestbaud.c
librtemscpu_a_SOURCES += libcsupport/src/termios_setinitialbaud.c
librtemscpu_a_SOURCES += libcsupport/src/__times.c
librtemscpu_a_SOURCES += libcsupport/src/truncate.c
librtemscpu_a_SOURCES += libcsupport/src/uenvgetgroups.c
librtemscpu_a_SOURCES += libcsupport/src/umask.c
librtemscpu_a_SOURCES += libcsupport/src/unlink.c
librtemscpu_a_SOURCES += libcsupport/src/unmount.c
librtemscpu_a_SOURCES += libcsupport/src/__usrenv.c
librtemscpu_a_SOURCES += libcsupport/src/utime.c
librtemscpu_a_SOURCES += libcsupport/src/utimes.c
librtemscpu_a_SOURCES += libcsupport/src/utsname.c
librtemscpu_a_SOURCES += libcsupport/src/vprintk.c
librtemscpu_a_SOURCES += libcsupport/src/write.c
librtemscpu_a_SOURCES += libcsupport/src/write_r.c
librtemscpu_a_SOURCES += libcsupport/src/writev.c
librtemscpu_a_SOURCES += libmisc/bspcmdline/bspcmdline_get.c
librtemscpu_a_SOURCES += libmisc/bspcmdline/bspcmdline_getparam.c
librtemscpu_a_SOURCES += libmisc/bspcmdline/bspcmdline_getparamraw.c
librtemscpu_a_SOURCES += libmisc/bspcmdline/bspcmdline_getparamrhs.c
librtemscpu_a_SOURCES += libmisc/capture/capture_buffer.c
librtemscpu_a_SOURCES += libmisc/capture/capture.c
librtemscpu_a_SOURCES += libmisc/capture/capture-cli.c
librtemscpu_a_SOURCES += libmisc/capture/capture_support.c
librtemscpu_a_SOURCES += libmisc/capture/capture_user_extension.c
librtemscpu_a_SOURCES += libmisc/capture/rtems-trace-buffer-vars.c
librtemscpu_a_SOURCES += libmisc/cpuuse/cpuinforeport.c
librtemscpu_a_SOURCES += libmisc/cpuuse/cpuusagedata.c
librtemscpu_a_SOURCES += libmisc/cpuuse/cpuusagereport.c
librtemscpu_a_SOURCES += libmisc/cpuuse/cpuusagereset.c
librtemscpu_a_SOURCES += libmisc/cpuuse/cpuusagetop.c
librtemscpu_a_SOURCES += libmisc/devnull/devnull.c
librtemscpu_a_SOURCES += libmisc/devnull/devzero.c
librtemscpu_a_SOURCES += libmisc/dummy/dummy-networking.c
librtemscpu_a_SOURCES += libmisc/dumpbuf/dumpbuf.c
librtemscpu_a_SOURCES += libmisc/fb/mw_print.c
librtemscpu_a_SOURCES += libmisc/fb/mw_uid.c
librtemscpu_a_SOURCES += libmisc/fsmount/fsmount.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-command.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-config.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-driver.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-editor.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-extension.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-itask.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-manager.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-monitor.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-object.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-part.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-prmisc.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-queue.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-region.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-sema.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-server.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-symbols.c
librtemscpu_a_SOURCES += libmisc/monitor/mon-task.c
librtemscpu_a_SOURCES += libmisc/mouse/mouse_parser.c
librtemscpu_a_SOURCES += libmisc/mouse/serial_mouse.c
librtemscpu_a_SOURCES += libmisc/redirector/stdio-redirect.c
librtemscpu_a_SOURCES += libmisc/rtems-fdt/rtems-fdt.c
librtemscpu_a_SOURCES += libmisc/rtems-fdt/rtems-fdt-shell.c
librtemscpu_a_SOURCES += libmisc/stackchk/check.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtodouble.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtofloat.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtoint.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtolong.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtolongdouble.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtolonglong.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtopointer.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtounsignedchar.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtounsignedint.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtounsignedlong.c
librtemscpu_a_SOURCES += libmisc/stringto/stringtounsignedlonglong.c
librtemscpu_a_SOURCES += libmisc/untar/untar.c
librtemscpu_a_SOURCES += libmisc/untar/untar_tgz.c
librtemscpu_a_SOURCES += libmisc/untar/untar_txz.c
librtemscpu_a_SOURCES += libmisc/uuid/clear.c
librtemscpu_a_SOURCES += libmisc/uuid/compare.c
librtemscpu_a_SOURCES += libmisc/uuid/copy.c
librtemscpu_a_SOURCES += libmisc/uuid/gen_uuid.c
librtemscpu_a_SOURCES += libmisc/uuid/isnull.c
librtemscpu_a_SOURCES += libmisc/uuid/pack.c
librtemscpu_a_SOURCES += libmisc/uuid/parse.c
librtemscpu_a_SOURCES += libmisc/uuid/unpack.c
librtemscpu_a_SOURCES += libmisc/uuid/unparse.c
librtemscpu_a_SOURCES += libmisc/uuid/uuid_time.c
librtemscpu_a_SOURCES += libmisc/xz/xz_crc32.c
librtemscpu_a_SOURCES += libmisc/xz/xz_dec_lzma2.c
librtemscpu_a_SOURCES += libmisc/xz/xz_dec_stream.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_are_nodes_equal.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_chown.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_clone.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_close.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_eval_path.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_fchmod.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_fcntl.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_freenode.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_fstat.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_fsunmount.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_fsync.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_fsync_success.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_ftruncate.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_ftruncate_directory.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_handlers.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_ioctl.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_kqfilter.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_link.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_lock_and_unlock.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_lseek.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_lseek_directory.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_lseek_file.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_mknod.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_mmap.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_mount.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_open.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_ops.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_poll.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_read.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_readlink.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_readv.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_rename.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_rmnod.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_statvfs.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_symlink.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_unmount.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_utime.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_write.c
librtemscpu_a_SOURCES += libfs/src/defaults/default_writev.c
librtemscpu_a_SOURCES += libfs/src/dosfs/fat.c
librtemscpu_a_SOURCES += libfs/src/dosfs/fat_fat_operations.c
librtemscpu_a_SOURCES += libfs/src/dosfs/fat_file.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_conv.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_conv_default.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_conv_utf8.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_create.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_dir.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_eval.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_file.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_format.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_free.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_fsunmount.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_handlers_dir.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_handlers_file.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_init.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_initsupp.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_misc.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_mknod.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_rename.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_rmnod.c
librtemscpu_a_SOURCES += libfs/src/dosfs/msdos_statvfs.c
librtemscpu_a_SOURCES += libfs/src/imfs/deviceio.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_add_node.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_chown.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_config.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_creat.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_dir.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_dir_default.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_dir_minimal.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_eval.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_eval_devfs.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_fchmod.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_fifo.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_fsunmount.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_handlers_device.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_init.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_initsupp.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_linfile.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_link.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_load_tar.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_make_generic_node.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_make_linfile.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_memfile.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_mknod.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_mount.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_node.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_node_destroy_default.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_rename.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_rmnod.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_stat.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_stat_file.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_symlink.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_unmount.c
librtemscpu_a_SOURCES += libfs/src/imfs/imfs_utime.c
librtemscpu_a_SOURCES += libfs/src/imfs/ioman.c
librtemscpu_a_SOURCES += libfs/src/pipe/fifo.c
librtemscpu_a_SOURCES += libfs/src/pipe/pipe.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-bitmaps.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-block.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-buffer-bdbuf.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-buffer.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-dir.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-dir-hash.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-file.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-file-system.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-format.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-group.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-inode.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-link.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-mutex.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-rtems.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-rtems-dev.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-rtems-dir.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-rtems-file.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-rtems-utils.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-shell.c
librtemscpu_a_SOURCES += libfs/src/rfs/rtems-rfs-trace.c
librtemscpu_a_SOURCES += libi2c/libi2c.c
librtemscpu_a_SOURCES += libmd/md4.c
librtemscpu_a_SOURCES += libmd/md5.c
librtemscpu_a_SOURCES += libmd/sha256c.c
librtemscpu_a_SOURCES += libmd/sha512c.c
librtemscpu_a_SOURCES += libstdthreads/call_once.c
librtemscpu_a_SOURCES += libstdthreads/cnd.c
librtemscpu_a_SOURCES += libstdthreads/mtx.c
librtemscpu_a_SOURCES += libstdthreads/thrd.c
librtemscpu_a_SOURCES += libstdthreads/tss.c
librtemscpu_a_SOURCES += libtrace/record/record.c
librtemscpu_a_SOURCES += libtrace/record/record-client.c
librtemscpu_a_SOURCES += libtrace/record/record-dump.c
librtemscpu_a_SOURCES += libtrace/record/record-dump-fatal.c
librtemscpu_a_SOURCES += libtrace/record/record-dump-zfatal.c
librtemscpu_a_SOURCES += libtrace/record/record-dump-base64.c
librtemscpu_a_SOURCES += libtrace/record/record-dump-zbase64.c
librtemscpu_a_SOURCES += libtrace/record/record-server.c
librtemscpu_a_SOURCES += libtrace/record/record-sysinit.c
librtemscpu_a_SOURCES += libtrace/record/record-text.c
librtemscpu_a_SOURCES += libtrace/record/record-userext.c
librtemscpu_a_SOURCES += libtrace/record/record-util.c
librtemscpu_a_SOURCES += posix/src/adjtime.c
librtemscpu_a_SOURCES += posix/src/aio_suspend.c
librtemscpu_a_SOURCES += posix/src/barrierattrdestroy.c
librtemscpu_a_SOURCES += posix/src/barrierattrgetpshared.c
librtemscpu_a_SOURCES += posix/src/barrierattrinit.c
librtemscpu_a_SOURCES += posix/src/barrierattrsetpshared.c
librtemscpu_a_SOURCES += posix/src/cancel.c
librtemscpu_a_SOURCES += posix/src/cleanuppush.c
librtemscpu_a_SOURCES += posix/src/clockgetcpuclockid.c
librtemscpu_a_SOURCES += posix/src/clockgetres.c
librtemscpu_a_SOURCES += posix/src/clockgettime.c
librtemscpu_a_SOURCES += posix/src/clocksettime.c
librtemscpu_a_SOURCES += posix/src/condattrdestroy.c
librtemscpu_a_SOURCES += posix/src/condattrgetclock.c
librtemscpu_a_SOURCES += posix/src/condattrgetpshared.c
librtemscpu_a_SOURCES += posix/src/condattrinit.c
librtemscpu_a_SOURCES += posix/src/condattrsetclock.c
librtemscpu_a_SOURCES += posix/src/condattrsetpshared.c
librtemscpu_a_SOURCES += posix/src/condbroadcast.c
librtemscpu_a_SOURCES += posix/src/conddefaultattributes.c
librtemscpu_a_SOURCES += posix/src/conddestroy.c
librtemscpu_a_SOURCES += posix/src/condinit.c
librtemscpu_a_SOURCES += posix/src/condsignal.c
librtemscpu_a_SOURCES += posix/src/condsignalsupp.c
librtemscpu_a_SOURCES += posix/src/condtimedwait.c
librtemscpu_a_SOURCES += posix/src/condwait.c
librtemscpu_a_SOURCES += posix/src/condwaitsupp.c
librtemscpu_a_SOURCES += posix/src/_execve.c
librtemscpu_a_SOURCES += posix/src/fork.c
librtemscpu_a_SOURCES += posix/src/key.c
librtemscpu_a_SOURCES += posix/src/keycreate.c
librtemscpu_a_SOURCES += posix/src/keydelete.c
librtemscpu_a_SOURCES += posix/src/keygetspecific.c
librtemscpu_a_SOURCES += posix/src/keysetspecific.c
librtemscpu_a_SOURCES += posix/src/keyzerokvp.c
librtemscpu_a_SOURCES += posix/src/lio_listio.c
librtemscpu_a_SOURCES += posix/src/mlockall.c
librtemscpu_a_SOURCES += posix/src/mlock.c
librtemscpu_a_SOURCES += posix/src/mmap.c
librtemscpu_a_SOURCES += posix/src/mprotect.c
librtemscpu_a_SOURCES += posix/src/mqueue.c
librtemscpu_a_SOURCES += posix/src/mqueueclose.c
librtemscpu_a_SOURCES += posix/src/mqueueconfig.c
librtemscpu_a_SOURCES += posix/src/mqueuedeletesupp.c
librtemscpu_a_SOURCES += posix/src/mqueuegetattr.c
librtemscpu_a_SOURCES += posix/src/mqueueopen.c
librtemscpu_a_SOURCES += posix/src/mqueuereceive.c
librtemscpu_a_SOURCES += posix/src/mqueuerecvsupp.c
librtemscpu_a_SOURCES += posix/src/mqueuesend.c
librtemscpu_a_SOURCES += posix/src/mqueuesendsupp.c
librtemscpu_a_SOURCES += posix/src/mqueuesetattr.c
librtemscpu_a_SOURCES += posix/src/mqueuetimedreceive.c
librtemscpu_a_SOURCES += posix/src/mqueuetimedsend.c
librtemscpu_a_SOURCES += posix/src/mqueueunlink.c
librtemscpu_a_SOURCES += posix/src/msync.c
librtemscpu_a_SOURCES += posix/src/munlockall.c
librtemscpu_a_SOURCES += posix/src/munlock.c
librtemscpu_a_SOURCES += posix/src/munmap.c
librtemscpu_a_SOURCES += posix/src/mutexattrdestroy.c
librtemscpu_a_SOURCES += posix/src/mutexattrgetprioceiling.c
librtemscpu_a_SOURCES += posix/src/mutexattrgetprotocol.c
librtemscpu_a_SOURCES += posix/src/mutexattrgetpshared.c
librtemscpu_a_SOURCES += posix/src/mutexattrgettype.c
librtemscpu_a_SOURCES += posix/src/mutexattrinit.c
librtemscpu_a_SOURCES += posix/src/mutexattrsetprioceiling.c
librtemscpu_a_SOURCES += posix/src/mutexattrsetprotocol.c
librtemscpu_a_SOURCES += posix/src/mutexattrsetpshared.c
librtemscpu_a_SOURCES += posix/src/mutexattrsettype.c
librtemscpu_a_SOURCES += posix/src/mutexdestroy.c
librtemscpu_a_SOURCES += posix/src/mutexgetprioceiling.c
librtemscpu_a_SOURCES += posix/src/mutexinit.c
librtemscpu_a_SOURCES += posix/src/mutexlock.c
librtemscpu_a_SOURCES += posix/src/mutexlocksupp.c
librtemscpu_a_SOURCES += posix/src/mutexsetprioceiling.c
librtemscpu_a_SOURCES += posix/src/mutextimedlock.c
librtemscpu_a_SOURCES += posix/src/mutextrylock.c
librtemscpu_a_SOURCES += posix/src/mutexunlock.c
librtemscpu_a_SOURCES += posix/src/nanosleep.c
librtemscpu_a_SOURCES += posix/src/pbarrierdestroy.c
librtemscpu_a_SOURCES += posix/src/pbarrierinit.c
librtemscpu_a_SOURCES += posix/src/pbarrierwait.c
librtemscpu_a_SOURCES += posix/src/posix_madvise.c
librtemscpu_a_SOURCES += posix/src/prwlockdestroy.c
librtemscpu_a_SOURCES += posix/src/prwlockinit.c
librtemscpu_a_SOURCES += posix/src/prwlockrdlock.c
librtemscpu_a_SOURCES += posix/src/prwlocktimedrdlock.c
librtemscpu_a_SOURCES += posix/src/prwlocktimedwrlock.c
librtemscpu_a_SOURCES += posix/src/prwlocktryrdlock.c
librtemscpu_a_SOURCES += posix/src/prwlocktrywrlock.c
librtemscpu_a_SOURCES += posix/src/prwlockunlock.c
librtemscpu_a_SOURCES += posix/src/prwlockwrlock.c
librtemscpu_a_SOURCES += posix/src/psignalconfig.c
librtemscpu_a_SOURCES += posix/src/pspindestroy.c
librtemscpu_a_SOURCES += posix/src/pspininit.c
librtemscpu_a_SOURCES += posix/src/pspinlock.c
librtemscpu_a_SOURCES += posix/src/pspinunlock.c
librtemscpu_a_SOURCES += posix/src/psxnametoid.c
librtemscpu_a_SOURCES += posix/src/psxpriorityisvalid.c
librtemscpu_a_SOURCES += posix/src/psxsemaphore.c
librtemscpu_a_SOURCES += posix/src/psxsemaphoreconfig.c
librtemscpu_a_SOURCES += posix/src/psxtimerconfig.c
librtemscpu_a_SOURCES += posix/src/psxtransschedparam.c
librtemscpu_a_SOURCES += posix/src/pthreadatfork.c
librtemscpu_a_SOURCES += posix/src/pthreadattrdefault.c
librtemscpu_a_SOURCES += posix/src/pthreadattrdestroy.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetaffinitynp.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetdetachstate.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetguardsize.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetinheritsched.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetschedparam.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetschedpolicy.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetscope.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetstackaddr.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetstack.c
librtemscpu_a_SOURCES += posix/src/pthreadattrgetstacksize.c
librtemscpu_a_SOURCES += posix/src/pthreadattrinit.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetaffinitynp.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetdetachstate.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetguardsize.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetinheritsched.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetschedparam.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetschedpolicy.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetscope.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetstackaddr.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetstack.c
librtemscpu_a_SOURCES += posix/src/pthreadattrsetstacksize.c
librtemscpu_a_SOURCES += posix/src/pthread.c
librtemscpu_a_SOURCES += posix/src/pthreadconcurrency.c
librtemscpu_a_SOURCES += posix/src/pthreadconfig.c
librtemscpu_a_SOURCES += posix/src/pthreadcreate.c
librtemscpu_a_SOURCES += posix/src/pthreaddetach.c
librtemscpu_a_SOURCES += posix/src/pthreadequal.c
librtemscpu_a_SOURCES += posix/src/pthreadexit.c
librtemscpu_a_SOURCES += posix/src/pthreadgetaffinitynp.c
librtemscpu_a_SOURCES += posix/src/pthreadgetattrnp.c
librtemscpu_a_SOURCES += posix/src/pthreadgetcpuclockid.c
librtemscpu_a_SOURCES += posix/src/pthreadgetnamenp.c
librtemscpu_a_SOURCES += posix/src/pthreadgetschedparam.c
librtemscpu_a_SOURCES += posix/src/pthreadinitthreads.c
librtemscpu_a_SOURCES += posix/src/pthreadjoin.c
librtemscpu_a_SOURCES += posix/src/pthreadonce.c
librtemscpu_a_SOURCES += posix/src/pthreadself.c
librtemscpu_a_SOURCES += posix/src/pthreadsetaffinitynp.c
librtemscpu_a_SOURCES += posix/src/pthreadsetnamenp.c
librtemscpu_a_SOURCES += posix/src/pthreadsetschedparam.c
librtemscpu_a_SOURCES += posix/src/pthreadsetschedprio.c
librtemscpu_a_SOURCES += posix/src/rwlockattrdestroy.c
librtemscpu_a_SOURCES += posix/src/rwlockattrgetpshared.c
librtemscpu_a_SOURCES += posix/src/rwlockattrinit.c
librtemscpu_a_SOURCES += posix/src/rwlockattrsetpshared.c
librtemscpu_a_SOURCES += posix/src/sched_getparam.c
librtemscpu_a_SOURCES += posix/src/sched_getprioritymax.c
librtemscpu_a_SOURCES += posix/src/sched_getprioritymin.c
librtemscpu_a_SOURCES += posix/src/sched_getscheduler.c
librtemscpu_a_SOURCES += posix/src/sched_rr_get_interval.c
librtemscpu_a_SOURCES += posix/src/sched_setparam.c
librtemscpu_a_SOURCES += posix/src/sched_setscheduler.c
librtemscpu_a_SOURCES += posix/src/sched_yield.c
librtemscpu_a_SOURCES += posix/src/semaphoredeletesupp.c
librtemscpu_a_SOURCES += posix/src/semclose.c
librtemscpu_a_SOURCES += posix/src/semdestroy.c
librtemscpu_a_SOURCES += posix/src/semgetvalue.c
librtemscpu_a_SOURCES += posix/src/seminit.c
librtemscpu_a_SOURCES += posix/src/semopen.c
librtemscpu_a_SOURCES += posix/src/sempost.c
librtemscpu_a_SOURCES += posix/src/semtimedwait.c
librtemscpu_a_SOURCES += posix/src/semtrywait.c
librtemscpu_a_SOURCES += posix/src/semunlink.c
librtemscpu_a_SOURCES += posix/src/semwait.c
librtemscpu_a_SOURCES += posix/src/setcancelstate.c
librtemscpu_a_SOURCES += posix/src/setcanceltype.c
librtemscpu_a_SOURCES += posix/src/shm.c
librtemscpu_a_SOURCES += posix/src/shmconfig.c
librtemscpu_a_SOURCES += posix/src/shmheap.c
librtemscpu_a_SOURCES += posix/src/shmopen.c
librtemscpu_a_SOURCES += posix/src/shmops.c
librtemscpu_a_SOURCES += posix/src/shmunlink.c
librtemscpu_a_SOURCES += posix/src/shmwkspace.c
librtemscpu_a_SOURCES += posix/src/sigaddset.c
librtemscpu_a_SOURCES += posix/src/sigdelset.c
librtemscpu_a_SOURCES += posix/src/sigemptyset.c
librtemscpu_a_SOURCES += posix/src/sigfillset.c
librtemscpu_a_SOURCES += posix/src/sigismember.c
librtemscpu_a_SOURCES += posix/src/sigprocmask.c
librtemscpu_a_SOURCES += posix/src/sysconf.c
librtemscpu_a_SOURCES += posix/src/testcancel.c
librtemscpu_a_SOURCES += posix/src/vfork.c
librtemscpu_a_SOURCES += posix/src/wait.c
librtemscpu_a_SOURCES += posix/src/waitpid.c
librtemscpu_a_SOURCES += rtems/src/barrier.c
librtemscpu_a_SOURCES += rtems/src/barriercreate.c
librtemscpu_a_SOURCES += rtems/src/barrierdelete.c
librtemscpu_a_SOURCES += rtems/src/barrierident.c
librtemscpu_a_SOURCES += rtems/src/barrierrelease.c
librtemscpu_a_SOURCES += rtems/src/barrierwait.c
librtemscpu_a_SOURCES += rtems/src/clockgetsecondssinceepoch.c
librtemscpu_a_SOURCES += rtems/src/clockgettickspersecond.c
librtemscpu_a_SOURCES += rtems/src/clockgettod.c
librtemscpu_a_SOURCES += rtems/src/clockgettodtimeval.c
librtemscpu_a_SOURCES += rtems/src/clockgetuptime.c
librtemscpu_a_SOURCES += rtems/src/clockgetuptimenanoseconds.c
librtemscpu_a_SOURCES += rtems/src/clockgetuptimeseconds.c
librtemscpu_a_SOURCES += rtems/src/clockgetuptimetimeval.c
librtemscpu_a_SOURCES += rtems/src/clockset.c
librtemscpu_a_SOURCES += rtems/src/clocktick.c
librtemscpu_a_SOURCES += rtems/src/clocktodtoseconds.c
librtemscpu_a_SOURCES += rtems/src/clocktodvalidate.c
librtemscpu_a_SOURCES += rtems/src/dpmem.c
librtemscpu_a_SOURCES += rtems/src/dpmemcreate.c
librtemscpu_a_SOURCES += rtems/src/dpmemdelete.c
librtemscpu_a_SOURCES += rtems/src/dpmemexternal2internal.c
librtemscpu_a_SOURCES += rtems/src/dpmemident.c
librtemscpu_a_SOURCES += rtems/src/dpmeminternal2external.c
librtemscpu_a_SOURCES += rtems/src/eventreceive.c
librtemscpu_a_SOURCES += rtems/src/eventseize.c
librtemscpu_a_SOURCES += rtems/src/eventsend.c
librtemscpu_a_SOURCES += rtems/src/eventsurrender.c
librtemscpu_a_SOURCES += rtems/src/getapiconfig.c
librtemscpu_a_SOURCES += rtems/src/intrbody.c
librtemscpu_a_SOURCES += rtems/src/intrcatch.c
librtemscpu_a_SOURCES += rtems/src/modes.c
librtemscpu_a_SOURCES += rtems/src/msg.c
librtemscpu_a_SOURCES += rtems/src/msgqbroadcast.c
librtemscpu_a_SOURCES += rtems/src/msgqcreate.c
librtemscpu_a_SOURCES += rtems/src/msgqdelete.c
librtemscpu_a_SOURCES += rtems/src/msgqflush.c
librtemscpu_a_SOURCES += rtems/src/msgqgetnumberpending.c
librtemscpu_a_SOURCES += rtems/src/msgqident.c
librtemscpu_a_SOURCES += rtems/src/msgqreceive.c
librtemscpu_a_SOURCES += rtems/src/msgqsend.c
librtemscpu_a_SOURCES += rtems/src/msgqurgent.c
librtemscpu_a_SOURCES += rtems/src/part.c
librtemscpu_a_SOURCES += rtems/src/partcreate.c
librtemscpu_a_SOURCES += rtems/src/partdelete.c
librtemscpu_a_SOURCES += rtems/src/partgetbuffer.c
librtemscpu_a_SOURCES += rtems/src/partident.c
librtemscpu_a_SOURCES += rtems/src/partreturnbuffer.c
librtemscpu_a_SOURCES += rtems/src/ratemon.c
librtemscpu_a_SOURCES += rtems/src/ratemoncancel.c
librtemscpu_a_SOURCES += rtems/src/ratemoncreate.c
librtemscpu_a_SOURCES += rtems/src/ratemondelete.c
librtemscpu_a_SOURCES += rtems/src/ratemongetstatistics.c
librtemscpu_a_SOURCES += rtems/src/ratemongetstatus.c
librtemscpu_a_SOURCES += rtems/src/ratemonident.c
librtemscpu_a_SOURCES += rtems/src/ratemonperiod.c
librtemscpu_a_SOURCES += rtems/src/ratemonreportstatistics.c
librtemscpu_a_SOURCES += rtems/src/ratemonresetall.c
librtemscpu_a_SOURCES += rtems/src/ratemonresetstatistics.c
librtemscpu_a_SOURCES += rtems/src/ratemontimeout.c
librtemscpu_a_SOURCES += rtems/src/region.c
librtemscpu_a_SOURCES += rtems/src/regioncreate.c
librtemscpu_a_SOURCES += rtems/src/regiondelete.c
librtemscpu_a_SOURCES += rtems/src/regionextend.c
librtemscpu_a_SOURCES += rtems/src/regiongetfreeinfo.c
librtemscpu_a_SOURCES += rtems/src/regiongetinfo.c
librtemscpu_a_SOURCES += rtems/src/regiongetsegment.c
librtemscpu_a_SOURCES += rtems/src/regiongetsegmentsize.c
librtemscpu_a_SOURCES += rtems/src/regionident.c
librtemscpu_a_SOURCES += rtems/src/regionprocessqueue.c
librtemscpu_a_SOURCES += rtems/src/regionresizesegment.c
librtemscpu_a_SOURCES += rtems/src/regionreturnsegment.c
librtemscpu_a_SOURCES += rtems/src/rtemsbuildid.c
librtemscpu_a_SOURCES += rtems/src/rtemsbuildname.c
librtemscpu_a_SOURCES += rtems/src/rtemsmaxprio.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectapimaximumclass.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectapiminimumclass.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectgetapiclassname.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectgetapiname.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectgetclassicname.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectgetclassinfo.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectgetname.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectidapimaximum.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectidapiminimum.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectidgetapi.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectidgetclass.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectidgetindex.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectidgetnode.c
librtemscpu_a_SOURCES += rtems/src/rtemsobjectsetname.c
librtemscpu_a_SOURCES += rtems/src/rtemstimer.c
librtemscpu_a_SOURCES += rtems/src/scheduleraddprocessor.c
librtemscpu_a_SOURCES += rtems/src/schedulergetmaxprio.c
librtemscpu_a_SOURCES += rtems/src/schedulergetprocessorset.c
librtemscpu_a_SOURCES += rtems/src/scheduleridentbyprocessor.c
librtemscpu_a_SOURCES += rtems/src/scheduleridentbyprocessorset.c
librtemscpu_a_SOURCES += rtems/src/schedulerident.c
librtemscpu_a_SOURCES += rtems/src/schedulermapfromposix.c
librtemscpu_a_SOURCES += rtems/src/schedulermaptoposix.c
librtemscpu_a_SOURCES += rtems/src/schedulerremoveprocessor.c
librtemscpu_a_SOURCES += rtems/src/sem.c
librtemscpu_a_SOURCES += rtems/src/semcreate.c
librtemscpu_a_SOURCES += rtems/src/semdelete.c
librtemscpu_a_SOURCES += rtems/src/semflush.c
librtemscpu_a_SOURCES += rtems/src/semident.c
librtemscpu_a_SOURCES += rtems/src/semobtain.c
librtemscpu_a_SOURCES += rtems/src/semrelease.c
librtemscpu_a_SOURCES += rtems/src/semsetpriority.c
librtemscpu_a_SOURCES += rtems/src/signalcatch.c
librtemscpu_a_SOURCES += rtems/src/signalsend.c
librtemscpu_a_SOURCES += rtems/src/status.c
librtemscpu_a_SOURCES += rtems/src/statustext.c
librtemscpu_a_SOURCES += rtems/src/statustoerrno.c
librtemscpu_a_SOURCES += rtems/src/systemeventreceive.c
librtemscpu_a_SOURCES += rtems/src/systemeventsend.c
librtemscpu_a_SOURCES += rtems/src/taskcreate.c
librtemscpu_a_SOURCES += rtems/src/taskdelete.c
librtemscpu_a_SOURCES += rtems/src/taskexit.c
librtemscpu_a_SOURCES += rtems/src/taskgetaffinity.c
librtemscpu_a_SOURCES += rtems/src/taskgetpriority.c
librtemscpu_a_SOURCES += rtems/src/taskgetscheduler.c
librtemscpu_a_SOURCES += rtems/src/taskident.c
librtemscpu_a_SOURCES += rtems/src/taskinitdefault.c
librtemscpu_a_SOURCES += rtems/src/taskinitusers.c
librtemscpu_a_SOURCES += rtems/src/taskissuspended.c
librtemscpu_a_SOURCES += rtems/src/taskiterate.c
librtemscpu_a_SOURCES += rtems/src/taskmode.c
librtemscpu_a_SOURCES += rtems/src/taskrestart.c
librtemscpu_a_SOURCES += rtems/src/taskresume.c
librtemscpu_a_SOURCES += rtems/src/tasks.c
librtemscpu_a_SOURCES += rtems/src/taskself.c
librtemscpu_a_SOURCES += rtems/src/tasksetaffinity.c
librtemscpu_a_SOURCES += rtems/src/tasksetpriority.c
librtemscpu_a_SOURCES += rtems/src/tasksetscheduler.c
librtemscpu_a_SOURCES += rtems/src/taskstart.c
librtemscpu_a_SOURCES += rtems/src/tasksuspend.c
librtemscpu_a_SOURCES += rtems/src/taskwakeafter.c
librtemscpu_a_SOURCES += rtems/src/taskwakewhen.c
librtemscpu_a_SOURCES += rtems/src/timercancel.c
librtemscpu_a_SOURCES += rtems/src/timercreate.c
librtemscpu_a_SOURCES += rtems/src/timerdelete.c
librtemscpu_a_SOURCES += rtems/src/timerfireafter.c
librtemscpu_a_SOURCES += rtems/src/timerfirewhen.c
librtemscpu_a_SOURCES += rtems/src/timergetinfo.c
librtemscpu_a_SOURCES += rtems/src/timerident.c
librtemscpu_a_SOURCES += rtems/src/timerreset.c
librtemscpu_a_SOURCES += rtems/src/timerserver.c
librtemscpu_a_SOURCES += rtems/src/timerserverfireafter.c
librtemscpu_a_SOURCES += rtems/src/timerserverfirewhen.c
librtemscpu_a_SOURCES += rtems/src/workspace.c
librtemscpu_a_SOURCES += rtems/src/workspacegreedy.c
librtemscpu_a_SOURCES += score/src/allocatormutex.c
librtemscpu_a_SOURCES += score/src/apimutexisowner.c
librtemscpu_a_SOURCES += score/src/apimutexlock.c
librtemscpu_a_SOURCES += score/src/apimutexunlock.c
librtemscpu_a_SOURCES += score/src/corebarrier.c
librtemscpu_a_SOURCES += score/src/corebarrierrelease.c
librtemscpu_a_SOURCES += score/src/corebarrierwait.c
librtemscpu_a_SOURCES += score/src/coremsg.c
librtemscpu_a_SOURCES += score/src/coremsgbroadcast.c
librtemscpu_a_SOURCES += score/src/coremsgclose.c
librtemscpu_a_SOURCES += score/src/coremsgflush.c
librtemscpu_a_SOURCES += score/src/coremsgflushwait.c
librtemscpu_a_SOURCES += score/src/coremsginsert.c
librtemscpu_a_SOURCES += score/src/coremsgseize.c
librtemscpu_a_SOURCES += score/src/coremsgsubmit.c
librtemscpu_a_SOURCES += score/src/coremutexseize.c
librtemscpu_a_SOURCES += score/src/percpu.c
librtemscpu_a_SOURCES += score/src/percpuasm.c
librtemscpu_a_SOURCES += score/src/corerwlock.c
librtemscpu_a_SOURCES += score/src/corerwlockobtainread.c
librtemscpu_a_SOURCES += score/src/corerwlockobtainwrite.c
librtemscpu_a_SOURCES += score/src/corerwlockrelease.c
librtemscpu_a_SOURCES += score/src/coresem.c
librtemscpu_a_SOURCES += score/src/heap.c
librtemscpu_a_SOURCES += score/src/heapallocate.c
librtemscpu_a_SOURCES += score/src/heapextend.c
librtemscpu_a_SOURCES += score/src/heapfree.c
librtemscpu_a_SOURCES += score/src/heapsizeofuserarea.c
librtemscpu_a_SOURCES += score/src/heapwalk.c
librtemscpu_a_SOURCES += score/src/heapgetinfo.c
librtemscpu_a_SOURCES += score/src/heapgetfreeinfo.c
librtemscpu_a_SOURCES += score/src/heapresizeblock.c
librtemscpu_a_SOURCES += score/src/heapiterate.c
librtemscpu_a_SOURCES += score/src/heapgreedy.c
librtemscpu_a_SOURCES += score/src/heapnoextend.c
librtemscpu_a_SOURCES += score/src/memoryallocate.c
librtemscpu_a_SOURCES += score/src/memorydirtyfreeareas.c
librtemscpu_a_SOURCES += score/src/memoryfill.c
librtemscpu_a_SOURCES += score/src/memoryzerobeforeuse.c
librtemscpu_a_SOURCES += score/src/memoryzerofreeareas.c
librtemscpu_a_SOURCES += score/src/objectallocate.c
librtemscpu_a_SOURCES += score/src/objectallocatenone.c
librtemscpu_a_SOURCES += score/src/objectallocatestatic.c
librtemscpu_a_SOURCES += score/src/objectallocateunlimited.c
librtemscpu_a_SOURCES += score/src/objectclose.c
librtemscpu_a_SOURCES += score/src/objectextendinformation.c
librtemscpu_a_SOURCES += score/src/objectfree.c
librtemscpu_a_SOURCES += score/src/objectfreestatic.c
librtemscpu_a_SOURCES += score/src/objectgetnext.c
librtemscpu_a_SOURCES += score/src/objectinitializeinformation.c
librtemscpu_a_SOURCES += score/src/objectnametoid.c
librtemscpu_a_SOURCES += score/src/objectnametoidstring.c
librtemscpu_a_SOURCES += score/src/objectshrinkinformation.c
librtemscpu_a_SOURCES += score/src/objectgetnoprotection.c
librtemscpu_a_SOURCES += score/src/objectidtoname.c
librtemscpu_a_SOURCES += score/src/objectgetnameasstring.c
librtemscpu_a_SOURCES += score/src/objectsetname.c
librtemscpu_a_SOURCES += score/src/objectgetinfo.c
librtemscpu_a_SOURCES += score/src/objectgetinfoid.c
librtemscpu_a_SOURCES += score/src/objectapimaximumclass.c
librtemscpu_a_SOURCES += score/src/objectnamespaceremove.c
librtemscpu_a_SOURCES += score/src/objectactivecount.c
librtemscpu_a_SOURCES += score/src/objectgetlocal.c
librtemscpu_a_SOURCES += score/src/log2table.c
librtemscpu_a_SOURCES += score/src/scheduler.c
librtemscpu_a_SOURCES += score/src/schedulergetaffinity.c
librtemscpu_a_SOURCES += score/src/schedulersetaffinity.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultmappriority.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultnodedestroy.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultnodeinit.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultreleasejob.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultschedule.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultstartidle.c
librtemscpu_a_SOURCES += score/src/schedulerdefaulttick.c
librtemscpu_a_SOURCES += score/src/schedulerpriority.c
librtemscpu_a_SOURCES += score/src/schedulerpriorityblock.c
librtemscpu_a_SOURCES += score/src/schedulerprioritychangepriority.c
librtemscpu_a_SOURCES += score/src/schedulerpriorityschedule.c
librtemscpu_a_SOURCES += score/src/schedulerpriorityunblock.c
librtemscpu_a_SOURCES += score/src/schedulerpriorityyield.c
librtemscpu_a_SOURCES += score/src/schedulersimple.c
librtemscpu_a_SOURCES += score/src/schedulersimpleblock.c
librtemscpu_a_SOURCES += score/src/schedulersimplechangepriority.c
librtemscpu_a_SOURCES += score/src/schedulersimpleschedule.c
librtemscpu_a_SOURCES += score/src/schedulersimpleunblock.c
librtemscpu_a_SOURCES += score/src/schedulersimpleyield.c
librtemscpu_a_SOURCES += score/src/scheduleredf.c
librtemscpu_a_SOURCES += score/src/scheduleredfnodeinit.c
librtemscpu_a_SOURCES += score/src/scheduleredfblock.c
librtemscpu_a_SOURCES += score/src/scheduleredfchangepriority.c
librtemscpu_a_SOURCES += score/src/scheduleredfreleasejob.c
librtemscpu_a_SOURCES += score/src/scheduleredfschedule.c
librtemscpu_a_SOURCES += score/src/scheduleredfunblock.c
librtemscpu_a_SOURCES += score/src/scheduleredfyield.c
librtemscpu_a_SOURCES += score/src/schedulercbs.c
librtemscpu_a_SOURCES += score/src/schedulercbsnodeinit.c
librtemscpu_a_SOURCES += score/src/schedulercbsattachthread.c
librtemscpu_a_SOURCES += score/src/schedulercbscleanup.c
librtemscpu_a_SOURCES += score/src/schedulercbscreateserver.c
librtemscpu_a_SOURCES += score/src/schedulercbsdestroyserver.c
librtemscpu_a_SOURCES += score/src/schedulercbsdetachthread.c
librtemscpu_a_SOURCES += score/src/schedulercbsgetapprovedbudget.c
librtemscpu_a_SOURCES += score/src/schedulercbsgetexecutiontime.c
librtemscpu_a_SOURCES += score/src/schedulercbsgetparameters.c
librtemscpu_a_SOURCES += score/src/schedulercbsgetremainingbudget.c
librtemscpu_a_SOURCES += score/src/schedulercbsgetserverid.c
librtemscpu_a_SOURCES += score/src/schedulercbssetparameters.c
librtemscpu_a_SOURCES += score/src/schedulercbsreleasejob.c
librtemscpu_a_SOURCES += score/src/schedulercbsunblock.c
librtemscpu_a_SOURCES += score/src/stackallocator.c
librtemscpu_a_SOURCES += score/src/pheapallocate.c
librtemscpu_a_SOURCES += score/src/pheapextend.c
librtemscpu_a_SOURCES += score/src/pheapfree.c
librtemscpu_a_SOURCES += score/src/pheapgetsize.c
librtemscpu_a_SOURCES += score/src/pheapgetblocksize.c
librtemscpu_a_SOURCES += score/src/pheapgetfreeinfo.c
librtemscpu_a_SOURCES += score/src/pheapgetinfo.c
librtemscpu_a_SOURCES += score/src/pheapinit.c
librtemscpu_a_SOURCES += score/src/pheapresizeblock.c
librtemscpu_a_SOURCES += score/src/pheapwalk.c
librtemscpu_a_SOURCES += score/src/pheapiterate.c
librtemscpu_a_SOURCES += score/src/freechain.c
librtemscpu_a_SOURCES += score/src/rbtreeextract.c
librtemscpu_a_SOURCES += score/src/rbtreeinsert.c
librtemscpu_a_SOURCES += score/src/rbtreeiterate.c
librtemscpu_a_SOURCES += score/src/rbtreenext.c
librtemscpu_a_SOURCES += score/src/rbtreepostorder.c
librtemscpu_a_SOURCES += score/src/rbtreereplace.c
librtemscpu_a_SOURCES += score/src/threadallocateunlimited.c
librtemscpu_a_SOURCES += score/src/thread.c
librtemscpu_a_SOURCES += score/src/threadchangepriority.c
librtemscpu_a_SOURCES += score/src/threadclearstate.c
librtemscpu_a_SOURCES += score/src/threadcreateidle.c
librtemscpu_a_SOURCES += score/src/threaddispatch.c
librtemscpu_a_SOURCES += score/src/threadget.c
librtemscpu_a_SOURCES += score/src/threadhandler.c
librtemscpu_a_SOURCES += score/src/threadinitialize.c
librtemscpu_a_SOURCES += score/src/threadidledefault.c
librtemscpu_a_SOURCES += score/src/threadloadenv.c
librtemscpu_a_SOURCES += score/src/threadrestart.c
librtemscpu_a_SOURCES += score/src/threadsetstate.c
librtemscpu_a_SOURCES += score/src/threadstackallocate.c
librtemscpu_a_SOURCES += score/src/threadstackfree.c
librtemscpu_a_SOURCES += score/src/threadstart.c
librtemscpu_a_SOURCES += score/src/threadstartmultitasking.c
librtemscpu_a_SOURCES += score/src/iterateoverthreads.c
librtemscpu_a_SOURCES += score/src/threadentryadaptoridle.c
librtemscpu_a_SOURCES += score/src/threadentryadaptornumeric.c
librtemscpu_a_SOURCES += score/src/threadentryadaptorpointer.c
librtemscpu_a_SOURCES += score/src/threadgetcputimeused.c
librtemscpu_a_SOURCES += score/src/threaditerate.c
librtemscpu_a_SOURCES += score/src/threadname.c
librtemscpu_a_SOURCES += score/src/threadscheduler.c
librtemscpu_a_SOURCES += score/src/threadtimeout.c
librtemscpu_a_SOURCES += score/src/threadwaitgetid.c
librtemscpu_a_SOURCES += score/src/threadyield.c
librtemscpu_a_SOURCES += score/src/threadq.c
librtemscpu_a_SOURCES += score/src/threadqenqueue.c
librtemscpu_a_SOURCES += score/src/threadqextractwithproxy.c
librtemscpu_a_SOURCES += score/src/threadqfirst.c
librtemscpu_a_SOURCES += score/src/threadqflush.c
librtemscpu_a_SOURCES += score/src/threadqops.c
librtemscpu_a_SOURCES += score/src/threadqtimeout.c
librtemscpu_a_SOURCES += score/src/timespecaddto.c
librtemscpu_a_SOURCES += score/src/timespecfromticks.c
librtemscpu_a_SOURCES += score/src/timespecisvalid.c
librtemscpu_a_SOURCES += score/src/timespeclessthan.c
librtemscpu_a_SOURCES += score/src/timespecsubtract.c
librtemscpu_a_SOURCES += score/src/timespectoticks.c
librtemscpu_a_SOURCES += score/src/timespecdivide.c
librtemscpu_a_SOURCES += score/src/timespecdividebyinteger.c
librtemscpu_a_SOURCES += score/src/timespecgetasnanoseconds.c
librtemscpu_a_SOURCES += score/src/coretod.c
librtemscpu_a_SOURCES += score/src/coretodset.c
librtemscpu_a_SOURCES += score/src/coretodtickspersec.c
librtemscpu_a_SOURCES += score/src/coretodadjust.c
librtemscpu_a_SOURCES += score/src/watchdoginsert.c
librtemscpu_a_SOURCES += score/src/coretodhookdata.c
librtemscpu_a_SOURCES += score/src/coretodhookregister.c
librtemscpu_a_SOURCES += score/src/coretodhookrun.c
librtemscpu_a_SOURCES += score/src/coretodhookunregister.c
librtemscpu_a_SOURCES += score/src/watchdogremove.c
librtemscpu_a_SOURCES += score/src/watchdogtick.c
librtemscpu_a_SOURCES += score/src/watchdogtickssinceboot.c
librtemscpu_a_SOURCES += score/src/watchdogtimeslicedefault.c
librtemscpu_a_SOURCES += score/src/userextaddset.c
librtemscpu_a_SOURCES += score/src/userext.c
librtemscpu_a_SOURCES += score/src/userextremoveset.c
librtemscpu_a_SOURCES += score/src/userextiterate.c
librtemscpu_a_SOURCES += score/src/chain.c
librtemscpu_a_SOURCES += score/src/chainnodecount.c
librtemscpu_a_SOURCES += score/src/debugisthreaddispatchingallowed.c
librtemscpu_a_SOURCES += score/src/interr.c
librtemscpu_a_SOURCES += score/src/isr.c
librtemscpu_a_SOURCES += score/src/wkspace.c
librtemscpu_a_SOURCES += score/src/wkspaceisunifieddefault.c
librtemscpu_a_SOURCES += score/src/wkstringduplicate.c
librtemscpu_a_SOURCES += score/src/iobase64.c
librtemscpu_a_SOURCES += score/src/ioprintf.c
librtemscpu_a_SOURCES += score/src/iovprintf.c
librtemscpu_a_SOURCES += score/src/isrisinprogress.c
librtemscpu_a_SOURCES += score/src/condition.c
librtemscpu_a_SOURCES += score/src/configstackspacesize.c
librtemscpu_a_SOURCES += score/src/futex.c
librtemscpu_a_SOURCES += score/src/profilingisrentryexit.c
librtemscpu_a_SOURCES += score/src/mutex.c
librtemscpu_a_SOURCES += score/src/once.c
librtemscpu_a_SOURCES += score/src/sched.c
librtemscpu_a_SOURCES += score/src/semaphore.c
librtemscpu_a_SOURCES += score/src/smpbarrierwait.c
librtemscpu_a_SOURCES += score/src/kern_tc.c
librtemscpu_a_SOURCES += score/src/libatomic.c
librtemscpu_a_SOURCES += score/src/processormaskcopy.c
librtemscpu_a_SOURCES += score/src/tlsallocsize.c
librtemscpu_a_SOURCES += sapi/src/chainappendnotify.c
librtemscpu_a_SOURCES += sapi/src/chaingetnotify.c
librtemscpu_a_SOURCES += sapi/src/chaingetwait.c
librtemscpu_a_SOURCES += sapi/src/chainprependnotify.c
librtemscpu_a_SOURCES += sapi/src/chainprotected.c
librtemscpu_a_SOURCES += sapi/src/cpucounterconverter.c
librtemscpu_a_SOURCES += sapi/src/delaynano.c
librtemscpu_a_SOURCES += sapi/src/delayticks.c
librtemscpu_a_SOURCES += sapi/src/exinit.c
librtemscpu_a_SOURCES += sapi/src/exshutdown.c
librtemscpu_a_SOURCES += sapi/src/extension.c
librtemscpu_a_SOURCES += sapi/src/extensioncreate.c
librtemscpu_a_SOURCES += sapi/src/extensiondelete.c
librtemscpu_a_SOURCES += sapi/src/extensionident.c
librtemscpu_a_SOURCES += sapi/src/fatal.c
librtemscpu_a_SOURCES += sapi/src/fatalsrctext.c
librtemscpu_a_SOURCES += sapi/src/getconfigmax.c
librtemscpu_a_SOURCES += sapi/src/getcopyrightnotice.c
librtemscpu_a_SOURCES += sapi/src/getversionstring.c
librtemscpu_a_SOURCES += sapi/src/interrtext.c
librtemscpu_a_SOURCES += sapi/src/io.c
librtemscpu_a_SOURCES += sapi/src/iodefault.c
librtemscpu_a_SOURCES += sapi/src/ioclose.c
librtemscpu_a_SOURCES += sapi/src/iocontrol.c
librtemscpu_a_SOURCES += sapi/src/ioinitialize.c
librtemscpu_a_SOURCES += sapi/src/ioopen.c
librtemscpu_a_SOURCES += sapi/src/ioread.c
librtemscpu_a_SOURCES += sapi/src/ioregisterdriver.c
librtemscpu_a_SOURCES += sapi/src/iounregisterdriver.c
librtemscpu_a_SOURCES += sapi/src/iowrite.c
librtemscpu_a_SOURCES += sapi/src/panic.c
librtemscpu_a_SOURCES += sapi/src/posixapi.c
librtemscpu_a_SOURCES += sapi/src/profilingiterate.c
librtemscpu_a_SOURCES += sapi/src/profilingreportxml.c
librtemscpu_a_SOURCES += sapi/src/rbheap.c
librtemscpu_a_SOURCES += sapi/src/rbtree.c
librtemscpu_a_SOURCES += sapi/src/rbtreefind.c
librtemscpu_a_SOURCES += sapi/src/sapirbtreeinsert.c
librtemscpu_a_SOURCES += sapi/src/sysinitverbose.c
librtemscpu_a_SOURCES += sapi/src/tcsimpleinstall.c
librtemscpu_a_SOURCES += sapi/src/version.c

if HAS_MP

librtemscpu_a_SOURCES += libmisc/monitor/mon-mpci.c
librtemscpu_a_SOURCES += rtems/src/eventmp.c
librtemscpu_a_SOURCES += rtems/src/mp.c
librtemscpu_a_SOURCES += rtems/src/msgmp.c
librtemscpu_a_SOURCES += rtems/src/partmp.c
librtemscpu_a_SOURCES += rtems/src/semmp.c
librtemscpu_a_SOURCES += rtems/src/signalmp.c
librtemscpu_a_SOURCES += rtems/src/taskmp.c
librtemscpu_a_SOURCES += score/src/mpci.c
librtemscpu_a_SOURCES += score/src/mpcidefault.c
librtemscpu_a_SOURCES += score/src/objectmp.c
librtemscpu_a_SOURCES += score/src/threadmp.c

endif

if HAS_PTHREADS

librtemscpu_a_SOURCES += posix/src/aio_cancel.c
librtemscpu_a_SOURCES += posix/src/aio_error.c
librtemscpu_a_SOURCES += posix/src/aio_fsync.c
librtemscpu_a_SOURCES += posix/src/aio_misc.c
librtemscpu_a_SOURCES += posix/src/aio_read.c
librtemscpu_a_SOURCES += posix/src/aio_return.c
librtemscpu_a_SOURCES += posix/src/aio_write.c
librtemscpu_a_SOURCES += posix/src/alarm.c
librtemscpu_a_SOURCES += posix/src/getitimer.c
librtemscpu_a_SOURCES += posix/src/kill.c
librtemscpu_a_SOURCES += posix/src/killinfo.c
librtemscpu_a_SOURCES += posix/src/kill_r.c
librtemscpu_a_SOURCES += posix/src/mqueuenotify.c
librtemscpu_a_SOURCES += posix/src/pause.c
librtemscpu_a_SOURCES += posix/src/psignal.c
librtemscpu_a_SOURCES += posix/src/psignalclearprocesssignals.c
librtemscpu_a_SOURCES += posix/src/psignalclearsignals.c
librtemscpu_a_SOURCES += posix/src/psignalsetprocesssignals.c
librtemscpu_a_SOURCES += posix/src/psignalunblockthread.c
librtemscpu_a_SOURCES += posix/src/psxpriorityisvalid.c
librtemscpu_a_SOURCES += posix/src/psxtimercreate.c
librtemscpu_a_SOURCES += posix/src/psxtimerdelete.c
librtemscpu_a_SOURCES += posix/src/pthreadkill.c
librtemscpu_a_SOURCES += posix/src/pthreadsigmask.c
librtemscpu_a_SOURCES += posix/src/ptimer.c
librtemscpu_a_SOURCES += posix/src/setitimer.c
librtemscpu_a_SOURCES += posix/src/sigaction.c
librtemscpu_a_SOURCES += posix/src/signal_2.c
librtemscpu_a_SOURCES += posix/src/sigpending.c
librtemscpu_a_SOURCES += posix/src/sigqueue.c
librtemscpu_a_SOURCES += posix/src/sigsuspend.c
librtemscpu_a_SOURCES += posix/src/sigtimedwait.c
librtemscpu_a_SOURCES += posix/src/sigwait.c
librtemscpu_a_SOURCES += posix/src/sigwaitinfo.c
librtemscpu_a_SOURCES += posix/src/timergetoverrun.c
librtemscpu_a_SOURCES += posix/src/timergettime.c
librtemscpu_a_SOURCES += posix/src/timersettime.c
librtemscpu_a_SOURCES += posix/src/ualarm.c

endif

if HAS_SMP

librtemscpu_a_SOURCES += score/src/percpustatewait.c
librtemscpu_a_SOURCES += score/src/profilingsmplock.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultpinunpin.c
librtemscpu_a_SOURCES += score/src/scheduleredfsmp.c
librtemscpu_a_SOURCES += score/src/schedulerpriorityaffinitysmp.c
librtemscpu_a_SOURCES += score/src/schedulerprioritysmp.c
librtemscpu_a_SOURCES += score/src/schedulersimplesmp.c
librtemscpu_a_SOURCES += score/src/schedulerstrongapa.c
librtemscpu_a_SOURCES += score/src/smp.c
librtemscpu_a_SOURCES += score/src/smplock.c
librtemscpu_a_SOURCES += score/src/smpmulticastaction.c
librtemscpu_a_SOURCES += score/src/smpunicastaction.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultaskforhelp.c
librtemscpu_a_SOURCES += score/src/schedulerdefaultsetaffinity.c
librtemscpu_a_SOURCES += score/src/schedulersmp.c
librtemscpu_a_SOURCES += score/src/schedulersmpstartidle.c
librtemscpu_a_SOURCES += score/src/threadunpin.c

endif

if LIBDL

librtemscpu_a_SOURCES += libdl/dlfcn.c
librtemscpu_a_SOURCES += libdl/dlfcn-shell.c
librtemscpu_a_SOURCES += libdl/fastlz.c
librtemscpu_a_SOURCES += libdl/rap.c
librtemscpu_a_SOURCES += libdl/rap-shell.c
librtemscpu_a_SOURCES += libdl/rtl-allocator.c
librtemscpu_a_SOURCES += libdl/rtl-alloc-heap.c
librtemscpu_a_SOURCES += libdl/rtl-archive.c
librtemscpu_a_SOURCES += libdl/rtl-bit-alloc.c
librtemscpu_a_SOURCES += libdl/rtl.c
librtemscpu_a_SOURCES += libdl/rtl-chain-iterator.c
librtemscpu_a_SOURCES += libdl/rtl-debugger.c
librtemscpu_a_SOURCES += libdl/rtl-elf.c
librtemscpu_a_SOURCES += libdl/rtl-error.c
librtemscpu_a_SOURCES += libdl/rtl-find-file.c
librtemscpu_a_SOURCES += libdl/rtl-mdreloc-@RTEMS_CPU@.c
librtemscpu_a_SOURCES += libdl/rtl-obj.c
librtemscpu_a_SOURCES += libdl/rtl-obj-cache.c
librtemscpu_a_SOURCES += libdl/rtl-obj-comp.c
librtemscpu_a_SOURCES += libdl/rtl-rap.c
librtemscpu_a_SOURCES += libdl/rtl-shell.c
librtemscpu_a_SOURCES += libdl/rtl-string.c
librtemscpu_a_SOURCES += libdl/rtl-sym.c
librtemscpu_a_SOURCES += libdl/rtl-trace.c
librtemscpu_a_SOURCES += libdl/rtl-unresolved.c
librtemscpu_a_SOURCES += libdl/rtl-unwind-dw2.c

endif

if LIBDRVMGR

librtemscpu_a_SOURCES += libdrvmgr/drvmgr_by_id.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_by_name.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_dev_by_name.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_drvinf.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_for_each_dev.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_for_each_list_dev.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_func.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_func_call.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_init.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_list.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_lock.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_print.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_res.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_rw.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_translate.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_translate_check.c
librtemscpu_a_SOURCES += libdrvmgr/drvmgr_unregister.c

endif

if LIBGNAT

librtemscpu_a_SOURCES += libgnat/ada_intrsupp.c
librtemscpu_a_SOURCES += libgnat/adasupp.c

endif

if LIBNETWORKING

librtemscpu_a_SOURCES += libmisc/monitor/mon-network.c
librtemscpu_a_SOURCES += libnetworking/kern/kern_mib.c
librtemscpu_a_SOURCES += libnetworking/kern/kern_subr.c
librtemscpu_a_SOURCES += libnetworking/kern/kern_sysctl.c
librtemscpu_a_SOURCES += libnetworking/kern/uipc_domain.c
librtemscpu_a_SOURCES += libnetworking/kern/uipc_mbuf.c
librtemscpu_a_SOURCES += libnetworking/kern/uipc_socket2.c
librtemscpu_a_SOURCES += libnetworking/kern/uipc_socket.c
librtemscpu_a_SOURCES += libnetworking/libc/base64.c
librtemscpu_a_SOURCES += libnetworking/libc/gethostbydns.c
librtemscpu_a_SOURCES += libnetworking/libc/gethostbyht.c
librtemscpu_a_SOURCES += libnetworking/libc/gethostbynis.c
librtemscpu_a_SOURCES += libnetworking/libc/gethostnamadr.c
librtemscpu_a_SOURCES += libnetworking/libc/getifaddrs.c
librtemscpu_a_SOURCES += libnetworking/libc/getnameinfo.c
librtemscpu_a_SOURCES += libnetworking/libc/getnetbydns.c
librtemscpu_a_SOURCES += libnetworking/libc/getnetbyht.c
librtemscpu_a_SOURCES += libnetworking/libc/getnetbynis.c
librtemscpu_a_SOURCES += libnetworking/libc/getnetnamadr.c
librtemscpu_a_SOURCES += libnetworking/libc/getproto.c
librtemscpu_a_SOURCES += libnetworking/libc/getprotoent.c
librtemscpu_a_SOURCES += libnetworking/libc/getprotoname.c
librtemscpu_a_SOURCES += libnetworking/libc/getservbyname.c
librtemscpu_a_SOURCES += libnetworking/libc/getservbyport.c
librtemscpu_a_SOURCES += libnetworking/libc/getservent.c
librtemscpu_a_SOURCES += libnetworking/libc/herror.c
librtemscpu_a_SOURCES += libnetworking/libc/if_indextoname.c
librtemscpu_a_SOURCES += libnetworking/libc/if_nameindex.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_addr.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_lnaof.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_makeaddr.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_netof.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_network.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_ntoa.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_ntop.c
librtemscpu_a_SOURCES += libnetworking/libc/inet_pton.c
librtemscpu_a_SOURCES += libnetworking/libc/linkaddr.c
librtemscpu_a_SOURCES += libnetworking/libc/map_v4v6.c
librtemscpu_a_SOURCES += libnetworking/libc/nsap_addr.c
librtemscpu_a_SOURCES += libnetworking/libc/ns_name.c
librtemscpu_a_SOURCES += libnetworking/libc/ns_netint.c
librtemscpu_a_SOURCES += libnetworking/libc/ns_parse.c
librtemscpu_a_SOURCES += libnetworking/libc/ns_print.c
librtemscpu_a_SOURCES += libnetworking/libc/ns_ttl.c
librtemscpu_a_SOURCES += libnetworking/libc/rcmd.c
librtemscpu_a_SOURCES += libnetworking/libc/recv.c
librtemscpu_a_SOURCES += libnetworking/libc/res_comp.c
librtemscpu_a_SOURCES += libnetworking/libc/res_data.c
librtemscpu_a_SOURCES += libnetworking/libc/res_debug.c
librtemscpu_a_SOURCES += libnetworking/libc/res_init.c
librtemscpu_a_SOURCES += libnetworking/libc/res_mkquery.c
librtemscpu_a_SOURCES += libnetworking/libc/res_mkupdate.c
librtemscpu_a_SOURCES += libnetworking/libc/res_query.c
librtemscpu_a_SOURCES += libnetworking/libc/res_send.c
librtemscpu_a_SOURCES += libnetworking/libc/res_stubs.c
librtemscpu_a_SOURCES += libnetworking/libc/res_update.c
librtemscpu_a_SOURCES += libnetworking/libc/send.c
librtemscpu_a_SOURCES += libnetworking/lib/getprotoby.c
librtemscpu_a_SOURCES += libnetworking/lib/rtems_bsdnet_ntp.c
librtemscpu_a_SOURCES += libnetworking/lib/syslog.c
librtemscpu_a_SOURCES += libnetworking/net/if.c
librtemscpu_a_SOURCES += libnetworking/net/if_ethersubr.c
librtemscpu_a_SOURCES += libnetworking/net/if_loop.c
librtemscpu_a_SOURCES += libnetworking/net/if_ppp.c
librtemscpu_a_SOURCES += libnetworking/netinet/if_ether.c
librtemscpu_a_SOURCES += libnetworking/netinet/igmp.c
librtemscpu_a_SOURCES += libnetworking/netinet/in.c
librtemscpu_a_SOURCES += libnetworking/netinet/in_cksum.c
librtemscpu_a_SOURCES += libnetworking/netinet/in_pcb.c
librtemscpu_a_SOURCES += libnetworking/netinet/in_proto.c
librtemscpu_a_SOURCES += libnetworking/netinet/in_rmx.c
librtemscpu_a_SOURCES += libnetworking/netinet/ip_divert.c
librtemscpu_a_SOURCES += libnetworking/netinet/ip_fw.c
librtemscpu_a_SOURCES += libnetworking/netinet/ip_icmp.c
librtemscpu_a_SOURCES += libnetworking/netinet/ip_input.c
librtemscpu_a_SOURCES += libnetworking/netinet/ip_mroute.c
librtemscpu_a_SOURCES += libnetworking/netinet/ip_output.c
librtemscpu_a_SOURCES += libnetworking/netinet/raw_ip.c
librtemscpu_a_SOURCES += libnetworking/netinet/tcp_debug.c
librtemscpu_a_SOURCES += libnetworking/netinet/tcp_input.c
librtemscpu_a_SOURCES += libnetworking/netinet/tcp_output.c
librtemscpu_a_SOURCES += libnetworking/netinet/tcp_subr.c
librtemscpu_a_SOURCES += libnetworking/netinet/tcp_timer.c
librtemscpu_a_SOURCES += libnetworking/netinet/tcp_usrreq.c
librtemscpu_a_SOURCES += libnetworking/netinet/udp_usrreq.c
librtemscpu_a_SOURCES += libnetworking/net/ppp_tty.c
librtemscpu_a_SOURCES += libnetworking/net/radix.c
librtemscpu_a_SOURCES += libnetworking/net/raw_cb.c
librtemscpu_a_SOURCES += libnetworking/net/raw_usrreq.c
librtemscpu_a_SOURCES += libnetworking/net/route.c
librtemscpu_a_SOURCES += libnetworking/net/rtsock.c
librtemscpu_a_SOURCES += libnetworking/net/slcompress.c
librtemscpu_a_SOURCES += libnetworking/nfs/bootp_subr.c
librtemscpu_a_SOURCES += libnetworking/rtems/mkrootfs.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_bootp.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_bsdnet_malloc_starvation.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_dhcp.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_dhcp_failsafe.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_glue.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_malloc_mbuf.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_mii_ioctl.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_mii_ioctl_kern.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_select.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_showicmpstat.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_showifstat.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_showipstat.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_showmbuf.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_showroute.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_showtcpstat.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_showudpstat.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_socketpair.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_syscall_api.c
librtemscpu_a_SOURCES += libnetworking/rtems/rtems_syscall.c
librtemscpu_a_SOURCES += libnetworking/rtems/sghostname.c
librtemscpu_a_SOURCES += librpc/src/rpc/auth_none.c
librtemscpu_a_SOURCES += librpc/src/rpc/auth_unix.c
librtemscpu_a_SOURCES += librpc/src/rpc/authunix_prot.c
librtemscpu_a_SOURCES += librpc/src/rpc/bindresvport.c
librtemscpu_a_SOURCES += librpc/src/rpc/clnt_generic.c
librtemscpu_a_SOURCES += librpc/src/rpc/clnt_perror.c
librtemscpu_a_SOURCES += librpc/src/rpc/clnt_raw.c
librtemscpu_a_SOURCES += librpc/src/rpc/clnt_simple.c
librtemscpu_a_SOURCES += librpc/src/rpc/clnt_tcp.c
librtemscpu_a_SOURCES += librpc/src/rpc/clnt_udp.c
librtemscpu_a_SOURCES += librpc/src/rpc/get_myaddress.c
librtemscpu_a_SOURCES += librpc/src/rpc/getrpcent.c
librtemscpu_a_SOURCES += librpc/src/rpc/getrpcport.c
librtemscpu_a_SOURCES += librpc/src/rpc/netname.c
librtemscpu_a_SOURCES += librpc/src/rpc/netnamer.c
librtemscpu_a_SOURCES += librpc/src/rpc/pmap_clnt.c
librtemscpu_a_SOURCES += librpc/src/rpc/pmap_getmaps.c
librtemscpu_a_SOURCES += librpc/src/rpc/pmap_getport.c
librtemscpu_a_SOURCES += librpc/src/rpc/pmap_prot2.c
librtemscpu_a_SOURCES += librpc/src/rpc/pmap_prot.c
librtemscpu_a_SOURCES += librpc/src/rpc/pmap_rmt.c
librtemscpu_a_SOURCES += librpc/src/rpc/rpc_callmsg.c
librtemscpu_a_SOURCES += librpc/src/rpc/rpc_commondata.c
librtemscpu_a_SOURCES += librpc/src/rpc/rpcdname.c
librtemscpu_a_SOURCES += librpc/src/rpc/rpc_dtablesize.c
librtemscpu_a_SOURCES += librpc/src/rpc/rpc_prot.c
librtemscpu_a_SOURCES += librpc/src/rpc/rtems_portmapper.c
librtemscpu_a_SOURCES += librpc/src/rpc/rtems_rpc.c
librtemscpu_a_SOURCES += librpc/src/rpc/rtime.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc_auth.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc_auth_unix.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc_raw.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc_run.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc_simple.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc_tcp.c
librtemscpu_a_SOURCES += librpc/src/rpc/svc_udp.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr_array.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr_float.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr_mem.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr_rec.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr_reference.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr_sizeof.c
librtemscpu_a_SOURCES += librpc/src/xdr/xdr_stdio.c

endif

if LIBPCI

librtemscpu_a_SOURCES += libpci/pci_access.c
librtemscpu_a_SOURCES += libpci/pci_access_func.c
librtemscpu_a_SOURCES += libpci/pci_access_io.c
librtemscpu_a_SOURCES += libpci/pci_access_mem_be.c
librtemscpu_a_SOURCES += libpci/pci_access_mem.c
librtemscpu_a_SOURCES += libpci/pci_access_mem_le.c
librtemscpu_a_SOURCES += libpci/pci_bus.c
librtemscpu_a_SOURCES += libpci/pci_cfg_auto.c
librtemscpu_a_SOURCES += libpci/pci_cfg.c
librtemscpu_a_SOURCES += libpci/pci_cfg_peripheral.c
librtemscpu_a_SOURCES += libpci/pci_cfg_print_code.c
librtemscpu_a_SOURCES += libpci/pci_cfg_read.c
librtemscpu_a_SOURCES += libpci/pci_cfg_static.c
librtemscpu_a_SOURCES += libpci/pci_find.c
librtemscpu_a_SOURCES += libpci/pci_find_dev.c
librtemscpu_a_SOURCES += libpci/pci_for_each.c
librtemscpu_a_SOURCES += libpci/pci_for_each_child.c
librtemscpu_a_SOURCES += libpci/pci_for_each_dev.c
librtemscpu_a_SOURCES += libpci/pci_get_dev.c
librtemscpu_a_SOURCES += libpci/pci_irq.c
librtemscpu_a_SOURCES += libpci/pci_print.c

endif

if LIBSHELL

librtemscpu_a_SOURCES += libmisc/shell/cat_file.c
librtemscpu_a_SOURCES += libmisc/shell/cmds.c
librtemscpu_a_SOURCES += libmisc/shell/main_alias.c
librtemscpu_a_SOURCES += libmisc/shell/main_cat.c
librtemscpu_a_SOURCES += libmisc/shell/main_cd.c
librtemscpu_a_SOURCES += libmisc/shell/cmp-ls.c
librtemscpu_a_SOURCES += libmisc/shell/main_chdir.c
librtemscpu_a_SOURCES += libmisc/shell/main_chmod.c
librtemscpu_a_SOURCES += libmisc/shell/main_chroot.c
librtemscpu_a_SOURCES += libmisc/shell/main_cp.c
librtemscpu_a_SOURCES += libmisc/shell/main_cpuuse.c
librtemscpu_a_SOURCES += libmisc/shell/main_date.c
librtemscpu_a_SOURCES += libmisc/shell/main_dir.c
librtemscpu_a_SOURCES += libmisc/shell/main_echo.c
librtemscpu_a_SOURCES += libmisc/shell/main_exit.c
librtemscpu_a_SOURCES += libmisc/shell/main_halt.c
librtemscpu_a_SOURCES += libmisc/shell/main_help.c
librtemscpu_a_SOURCES += libmisc/shell/main_id.c
librtemscpu_a_SOURCES += libmisc/shell/main_logoff.c
librtemscpu_a_SOURCES += libmisc/shell/main_ln.c
librtemscpu_a_SOURCES += libmisc/shell/main_ls.c
librtemscpu_a_SOURCES += libmisc/shell/main_mallocinfo.c
librtemscpu_a_SOURCES += libmisc/shell/main_md5.c
librtemscpu_a_SOURCES += libmisc/shell/main_mdump.c
librtemscpu_a_SOURCES += libmisc/shell/main_medit.c
librtemscpu_a_SOURCES += libmisc/shell/main_mfill.c
librtemscpu_a_SOURCES += libmisc/shell/main_mkdir.c
librtemscpu_a_SOURCES += libmisc/shell/main_mount.c
librtemscpu_a_SOURCES += libmisc/shell/main_mmove.c
librtemscpu_a_SOURCES += libmisc/shell/main_msdosfmt.c
librtemscpu_a_SOURCES += libmisc/shell/main_mv.c
librtemscpu_a_SOURCES += libmisc/shell/main_perioduse.c
librtemscpu_a_SOURCES += libmisc/shell/main_top.c
librtemscpu_a_SOURCES += libmisc/shell/main_pwd.c
librtemscpu_a_SOURCES += libmisc/shell/main_rm.c
librtemscpu_a_SOURCES += libmisc/shell/main_rmdir.c
librtemscpu_a_SOURCES += libmisc/shell/main_sleep.c
librtemscpu_a_SOURCES += libmisc/shell/main_stackuse.c
librtemscpu_a_SOURCES += libmisc/shell/main_tty.c
librtemscpu_a_SOURCES += libmisc/shell/main_umask.c
librtemscpu_a_SOURCES += libmisc/shell/main_unmount.c
librtemscpu_a_SOURCES += libmisc/shell/main_blksync.c
librtemscpu_a_SOURCES += libmisc/shell/main_whoami.c
librtemscpu_a_SOURCES += libmisc/shell/shell.c
librtemscpu_a_SOURCES += libmisc/shell/shell_cmdset.c
librtemscpu_a_SOURCES += libmisc/shell/shell_getchar.c
librtemscpu_a_SOURCES += libmisc/shell/shell_getprompt.c
librtemscpu_a_SOURCES += libmisc/shell/shellconfig.c
librtemscpu_a_SOURCES += libmisc/shell/shell_makeargs.c
librtemscpu_a_SOURCES += libmisc/shell/filemode.c
librtemscpu_a_SOURCES += libmisc/shell/pwcache.c
librtemscpu_a_SOURCES += libmisc/shell/print-ls.c
librtemscpu_a_SOURCES += libmisc/shell/write_file.c
librtemscpu_a_SOURCES += libmisc/shell/utils-cp.c
librtemscpu_a_SOURCES += libmisc/shell/utils-ls.c
librtemscpu_a_SOURCES += libmisc/shell/err.c
librtemscpu_a_SOURCES += libmisc/shell/errx.c
librtemscpu_a_SOURCES += libmisc/shell/verr.c
librtemscpu_a_SOURCES += libmisc/shell/vis.c
librtemscpu_a_SOURCES += libmisc/shell/verrx.c
librtemscpu_a_SOURCES += libmisc/shell/vwarn.c
librtemscpu_a_SOURCES += libmisc/shell/vwarnx.c
librtemscpu_a_SOURCES += libmisc/shell/warn.c
librtemscpu_a_SOURCES += libmisc/shell/warnx.c
librtemscpu_a_SOURCES += libmisc/shell/fts.c
librtemscpu_a_SOURCES += libmisc/shell/print_heapinfo.c
librtemscpu_a_SOURCES += libmisc/shell/main_wkspaceinfo.c
librtemscpu_a_SOURCES += libmisc/shell/shell_script.c
librtemscpu_a_SOURCES += libmisc/shell/login_prompt.c
librtemscpu_a_SOURCES += libmisc/shell/login_check.c
librtemscpu_a_SOURCES += libmisc/shell/fdisk.c
librtemscpu_a_SOURCES += libmisc/shell/main_rtc.c
librtemscpu_a_SOURCES += libmisc/shell/dd-args.c
librtemscpu_a_SOURCES += libmisc/shell/main_dd.c
librtemscpu_a_SOURCES += libmisc/shell/dd-conv.c
librtemscpu_a_SOURCES += libmisc/shell/dd-conv_tab.c
librtemscpu_a_SOURCES += libmisc/shell/dd-misc.c
librtemscpu_a_SOURCES += libmisc/shell/dd-position.c
librtemscpu_a_SOURCES += libmisc/shell/main_hexdump.c
librtemscpu_a_SOURCES += libmisc/shell/hexdump-conv.c
librtemscpu_a_SOURCES += libmisc/shell/hexdump-display.c
librtemscpu_a_SOURCES += libmisc/shell/hexdump-odsyntax.c
librtemscpu_a_SOURCES += libmisc/shell/hexdump-parse.c
librtemscpu_a_SOURCES += libmisc/shell/hexsyntax.c
librtemscpu_a_SOURCES += libmisc/shell/main_time.c
librtemscpu_a_SOURCES += libmisc/shell/main_mknod.c
librtemscpu_a_SOURCES += libmisc/shell/main_setenv.c
librtemscpu_a_SOURCES += libmisc/shell/main_getenv.c
librtemscpu_a_SOURCES += libmisc/shell/main_unsetenv.c
librtemscpu_a_SOURCES += libmisc/shell/main_mkrfs.c
librtemscpu_a_SOURCES += libmisc/shell/main_debugrfs.c
librtemscpu_a_SOURCES += libmisc/shell/main_df.c
librtemscpu_a_SOURCES += libmisc/shell/main_lsof.c
librtemscpu_a_SOURCES += libmisc/shell/main_edit.c
librtemscpu_a_SOURCES += libmisc/shell/main_blkstats.c
librtemscpu_a_SOURCES += libmisc/shell/main_rtrace.c
librtemscpu_a_SOURCES += libmisc/shell/shell-wait-for-input.c
librtemscpu_a_SOURCES += libmisc/shell/main_cmdls.c
librtemscpu_a_SOURCES += libmisc/shell/main_cmdchown.c
librtemscpu_a_SOURCES += libmisc/shell/main_cmdchmod.c
librtemscpu_a_SOURCES += libmisc/shell/main_cpuinfo.c
librtemscpu_a_SOURCES += libmisc/shell/main_profreport.c

if LIBDRVMGR

librtemscpu_a_SOURCES += libmisc/shell/main_drvmgr.c

endif

if LIBPCI

librtemscpu_a_SOURCES += libmisc/shell/main_pci.c

endif

if LIBNETWORKING

librtemscpu_a_SOURCES += libmisc/shell/main_ifconfig.c
librtemscpu_a_SOURCES += libmisc/shell/main_route.c
librtemscpu_a_SOURCES += libmisc/shell/main_netstats.c
librtemscpu_a_SOURCES += libmisc/shell/main_ping.c

endif

endif

if LIBUTF8PROC

librtemscpu_a_SOURCES += libmisc/utf8proc/utf8proc.c

endif

if CPU_ARM

include $(srcdir)/score/cpu/arm/headers.am

librtemscpu_a_SOURCES += score/cpu/arm/__aeabi_read_tp.c
librtemscpu_a_SOURCES += score/cpu/arm/arm-context-validate.S
librtemscpu_a_SOURCES += score/cpu/arm/arm-context-volatile-clobber.S
librtemscpu_a_SOURCES += score/cpu/arm/arm_exc_abort.S
librtemscpu_a_SOURCES += score/cpu/arm/arm-exception-default.c
librtemscpu_a_SOURCES += score/cpu/arm/arm-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/arm/arm_exc_interrupt.S
librtemscpu_a_SOURCES += score/cpu/arm/armv4-exception-default.S
librtemscpu_a_SOURCES += score/cpu/arm/armv4-sync-synchronize.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-context-initialize.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-context-restore.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-context-switch.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-exception-default.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-exception-handler-get.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-exception-handler-set.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-exception-priority-get.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-exception-priority-handler.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-exception-priority-set.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-initialize.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-isr-dispatch.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-isr-enter-leave.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-isr-level-get.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-isr-level-set.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-isr-vector-install.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7m-multitasking-start-stop.c
librtemscpu_a_SOURCES += score/cpu/arm/armv7-thread-idle.c
librtemscpu_a_SOURCES += score/cpu/arm/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/arm/cpu.c
librtemscpu_a_SOURCES += score/cpu/arm/__tls_get_addr.c

endif

if CPU_BFIN

include $(srcdir)/score/cpu/bfin/headers.am

librtemscpu_a_SOURCES += score/cpu/bfin/bfin-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/bfin/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/bfin/cpu.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c

endif

if CPU_EPIPHANY

include $(srcdir)/score/cpu/epiphany/headers.am

librtemscpu_a_SOURCES += score/cpu/epiphany/cpu.c
librtemscpu_a_SOURCES += score/cpu/epiphany/epiphany-context-initialize.c
librtemscpu_a_SOURCES += score/cpu/epiphany/epiphany-context-switch.S
librtemscpu_a_SOURCES += score/cpu/epiphany/epiphany-exception-handler.S
librtemscpu_a_SOURCES += score/cpu/epiphany/setjmp.S
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c

endif

if CPU_I386

include $(srcdir)/score/cpu/i386/headers.am

librtemscpu_a_SOURCES += score/cpu/i386/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/i386/cpu.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c

endif

if CPU_LM32

include $(srcdir)/score/cpu/lm32/headers.am

librtemscpu_a_SOURCES += score/cpu/lm32/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/lm32/cpu.c
librtemscpu_a_SOURCES += score/cpu/lm32/irq.c
librtemscpu_a_SOURCES += score/cpu/lm32/lm32-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c

endif

if CPU_M68K

include $(srcdir)/score/cpu/m68k/headers.am

librtemscpu_a_SOURCES += score/cpu/m68k/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/m68k/cpu.c
librtemscpu_a_SOURCES += score/cpu/m68k/m68k-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/m68k/__m68k_read_tp.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c

endif

if CPU_MIPS

include $(srcdir)/score/cpu/mips/headers.am

librtemscpu_a_SOURCES += score/cpu/mips/cpu.c
librtemscpu_a_SOURCES += score/cpu/mips/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c

endif

if CPU_MOXIE

include $(srcdir)/score/cpu/moxie/headers.am

librtemscpu_a_SOURCES += score/cpu/moxie/cpu.c
librtemscpu_a_SOURCES += score/cpu/moxie/moxie-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/moxie/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c

endif

if CPU_NIOS2

include $(srcdir)/score/cpu/nios2/headers.am

librtemscpu_a_SOURCES += score/cpu/nios2/nios2-context-initialize.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-context-switch.S
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-context-validate.S
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-context-volatile-clobber.S
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-eic-il-low-level.S
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-eic-rsie-low-level.S
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-fatal-halt.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-iic-low-level.S
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-iic-irq.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-initialize.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-isr-get-level.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-isr-install-vector.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-isr-is-in-progress.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-isr-set-level.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-mpu-add-region.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-mpu-configuration.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-mpu-descriptor.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-mpu-disable-protected.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-mpu-reset.c
librtemscpu_a_SOURCES += score/cpu/nios2/nios2-thread-dispatch-disabled.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c

endif

if CPU_NO_CPU

include $(srcdir)/score/cpu/no_cpu/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpu.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpu_asm.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c

endif

if CPU_OR1K

include $(srcdir)/score/cpu/or1k/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/or1k/cpu.c
librtemscpu_a_SOURCES += score/cpu/or1k/or1k-context-initialize.c
librtemscpu_a_SOURCES += score/cpu/or1k/or1k-context-switch.S
librtemscpu_a_SOURCES += score/cpu/or1k/or1k-context-validate.S
librtemscpu_a_SOURCES += score/cpu/or1k/or1k-context-volatile-clobber.S
librtemscpu_a_SOURCES += score/cpu/or1k/or1k-exception-default.c
librtemscpu_a_SOURCES += score/cpu/or1k/or1k-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/or1k/or1k-exception-handler-low.S

endif

if CPU_POWERPC

include $(srcdir)/score/cpu/powerpc/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c
librtemscpu_a_SOURCES += score/cpu/powerpc/cpu.c
librtemscpu_a_SOURCES += score/cpu/powerpc/ppc-context-volatile-clobber.S
librtemscpu_a_SOURCES += score/cpu/powerpc/ppc-context-validate.S
librtemscpu_a_SOURCES += score/cpu/powerpc/ppc-isr-disable-mask.S

endif

if CPU_RISCV

include $(srcdir)/score/cpu/riscv/headers.am

librtemscpu_a_SOURCES += score/cpu/riscv/cpu.c
librtemscpu_a_SOURCES += score/cpu/riscv/riscv-exception-handler.S
librtemscpu_a_SOURCES += score/cpu/riscv/riscv-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/riscv/riscv-context-switch.S
librtemscpu_a_SOURCES += score/cpu/riscv/riscv-context-initialize.c
librtemscpu_a_SOURCES += score/cpu/riscv/riscv-context-validate.S
librtemscpu_a_SOURCES += score/cpu/riscv/riscv-context-volatile-clobber.S
librtemscpu_a_SOURCES += score/cpu/riscv/riscv-counter.S

endif

if CPU_SH

include $(srcdir)/score/cpu/sh/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c
librtemscpu_a_SOURCES += score/cpu/sh/cpu.c
librtemscpu_a_SOURCES += score/cpu/sh/context.c
librtemscpu_a_SOURCES += score/cpu/sh/sh-exception-frame-print.c

endif

if CPU_SPARC64

include $(srcdir)/score/cpu/sparc64/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c
librtemscpu_a_SOURCES += score/cpu/sparc64/context.S
librtemscpu_a_SOURCES += score/cpu/sparc64/cpu.c
librtemscpu_a_SOURCES += score/cpu/sparc64/interrupt.S
librtemscpu_a_SOURCES += score/cpu/sparc64/sparc64-exception-frame-print.c
librtemscpu_a_SOURCES += score/cpu/sparc64/sparc64-syscall.S

endif

if CPU_SPARC

include $(srcdir)/score/cpu/sparc/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c
librtemscpu_a_SOURCES += score/cpu/sparc/access_le.c
librtemscpu_a_SOURCES += score/cpu/sparc/cpu.c
librtemscpu_a_SOURCES += score/cpu/sparc/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/sparc/sparc-access.S
librtemscpu_a_SOURCES += score/cpu/sparc/sparc-context-validate.S
librtemscpu_a_SOURCES += score/cpu/sparc/sparc-context-volatile-clobber.S
librtemscpu_a_SOURCES += score/cpu/sparc/sparc-counter-asm.S
librtemscpu_a_SOURCES += score/cpu/sparc/syscall.S
librtemscpu_a_SOURCES += score/cpu/sparc/window.S

endif

if CPU_V850

include $(srcdir)/score/cpu/v850/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c
librtemscpu_a_SOURCES += score/cpu/v850/cpu.c
librtemscpu_a_SOURCES += score/cpu/v850/cpu_asm.S
librtemscpu_a_SOURCES += score/cpu/v850/v850-exception-frame-print.c

endif

if CPU_X86_64

include $(srcdir)/score/cpu/x86_64/headers.am

librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterfrequency.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpucounterread.c
librtemscpu_a_SOURCES += score/cpu/no_cpu/cpuidle.c
librtemscpu_a_SOURCES += score/cpu/x86_64/cpu.c
librtemscpu_a_SOURCES += score/cpu/x86_64/x86_64-context-initialize.c
librtemscpu_a_SOURCES += score/cpu/x86_64/x86_64-context-switch.S

endif

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

project_lib_LIBRARIES += librtemsdefaultconfig.a

librtemsdefaultconfig_a_SOURCES =
librtemsdefaultconfig_a_SOURCES += libmisc/dummy/default-configuration.c

project_lib_LIBRARIES += librtemstest.a

librtemstest_a_SOURCES =
librtemstest_a_SOURCES += libtest/testbeginend.c
librtemstest_a_SOURCES += libtest/testbusy.c
librtemstest_a_SOURCES += libtest/testextension.c
librtemstest_a_SOURCES += libtest/testparallel.c
librtemstest_a_SOURCES += libtest/testwrappers.c
librtemstest_a_SOURCES += libtest/t-test.c
librtemstest_a_SOURCES += libtest/t-test-checks.c
librtemstest_a_SOURCES += libtest/t-test-checks-eno.c
librtemstest_a_SOURCES += libtest/t-test-checks-psx.c
librtemstest_a_SOURCES += libtest/t-test-hash-sha256.c
librtemstest_a_SOURCES += libtest/t-test-malloc.c
librtemstest_a_SOURCES += libtest/t-test-rtems.c
librtemstest_a_SOURCES += libtest/t-test-rtems-context.c
librtemstest_a_SOURCES += libtest/t-test-rtems-fds.c
librtemstest_a_SOURCES += libtest/t-test-rtems-heap.c
librtemstest_a_SOURCES += libtest/t-test-rtems-measure.c
librtemstest_a_SOURCES += libtest/t-test-rtems-objs.c
librtemstest_a_SOURCES += libtest/t-test-time.c

project_lib_LIBRARIES += libftpd.a

libftpd_a_SOURCES =
libftpd_a_SOURCES += ftpd/ftpd.c
libftpd_a_SOURCES += ftpd/ftpd-init.c

project_lib_LIBRARIES += libftpfs.a

libftpfs_a_SOURCES =
libftpfs_a_SOURCES += libnetworking/lib/ftpfs.c

project_lib_LIBRARIES += libtftpfs.a

libtftpfs_a_SOURCES =
libtftpfs_a_SOURCES += libnetworking/lib/tftpDriver.c

project_lib_LIBRARIES += libtelnetd.a

libtelnetd_a_SOURCES =
libtelnetd_a_SOURCES += telnetd/check_passwd.c
libtelnetd_a_SOURCES += telnetd/des.c
libtelnetd_a_SOURCES += telnetd/pty.c
libtelnetd_a_SOURCES += telnetd/telnetd.c
libtelnetd_a_SOURCES += telnetd/telnetd-init.c

if LIBDEBUGGER

project_lib_LIBRARIES += libdebugger.a

libdebugger_a_SOURCES =
libdebugger_a_SOURCES += libdebugger/rtems-debugger-block.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-bsp.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-cmd.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-remote.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-remote-tcp.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-@RTEMS_CPU@.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-server.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-target.c
libdebugger_a_SOURCES += libdebugger/rtems-debugger-threads.c

endif

project_lib_LIBRARIES += libjffs2.a

libjffs2_a_SOURCES =
libjffs2_a_SOURCES += libfs/src/jffs2/src/build.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/compat-crc32.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/compr.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/compr_rtime.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/compr_zlib.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/debug.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/dir-rtems.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/erase.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/flashio.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/fs-rtems.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/gc.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/malloc-rtems.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/nodelist.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/nodemgmt.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/read.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/readinode.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/scan.c
libjffs2_a_SOURCES += libfs/src/jffs2/src/write.c
libjffs2_a_CFLAGS =
libjffs2_a_CFLAGS += -Wno-pointer-sign
libjffs2_a_CPPFLAGS =
libjffs2_a_CPPFLAGS += $(AM_CPPFLAGS)
libjffs2_a_CPPFLAGS += -I$(srcdir)/libfs/src/jffs2/include

project_lib_LIBRARIES += libmghttpd.a

libmghttpd_a_SOURCES =
libmghttpd_a_SOURCES += mghttpd/mongoose.c

project_lib_LIBRARIES += libz.a

libz_a_SOURCES =
libz_a_SOURCES += zlib/adler32.c
libz_a_SOURCES += zlib/compress.c
libz_a_SOURCES += zlib/crc32.c
libz_a_SOURCES += zlib/deflate.c
libz_a_SOURCES += zlib/gzclose.c
libz_a_SOURCES += zlib/gzlib.c
libz_a_SOURCES += zlib/gzread.c
libz_a_SOURCES += zlib/gzwrite.c
libz_a_SOURCES += zlib/infback.c
libz_a_SOURCES += zlib/inffast.c
libz_a_SOURCES += zlib/inflate.c
libz_a_SOURCES += zlib/inftrees.c
libz_a_SOURCES += zlib/trees.c
libz_a_SOURCES += zlib/uncompr.c
libz_a_SOURCES += zlib/zutil.c

if LIBNETWORKING

project_lib_LIBRARIES += libnfs.a

libnfs_a_SOURCES =
libnfs_a_SOURCES += libfs/src/nfsclient/proto/mount_prot_xdr.c
libnfs_a_SOURCES += libfs/src/nfsclient/proto/nfs_prot_xdr.c
libnfs_a_SOURCES += libfs/src/nfsclient/src/nfs.c
libnfs_a_SOURCES += libfs/src/nfsclient/src/rpcio.c
libnfs_a_SOURCES += libfs/src/nfsclient/src/sock_mbuf.c
libnfs_a_SOURCES += libfs/src/nfsclient/src/xdr_mbuf.c

project_lib_LIBRARIES += libpppd.a

libpppd_a_SOURCES =
libpppd_a_SOURCES += pppd/auth.c
libpppd_a_SOURCES += pppd/ccp.c
libpppd_a_SOURCES += pppd/chap.c
libpppd_a_SOURCES += pppd/chap_ms.c
libpppd_a_SOURCES += pppd/chat.c
libpppd_a_SOURCES += pppd/demand.c
libpppd_a_SOURCES += pppd/fsm.c
libpppd_a_SOURCES += pppd/ipcp.c
libpppd_a_SOURCES += pppd/lcp.c
libpppd_a_SOURCES += pppd/magic.c
libpppd_a_SOURCES += pppd/options.c
libpppd_a_SOURCES += pppd/rtemsmain.c
libpppd_a_SOURCES += pppd/rtemspppd.c
libpppd_a_SOURCES += pppd/sys-rtems.c
libpppd_a_SOURCES += pppd/upap.c
libpppd_a_SOURCES += pppd/utils.c

endif

if HACK_TO_AVOID_LONG_ARG_LIST

librtemscpu.a: $(librtemscpu_a_OBJECTS) $(librtemscpu_a_DEPENDENCIES) $(EXTRA_librtemscpu_a_DEPENDENCIES)
	-rm -f librtemscpu.a librtemscpu.txt
	for i in $(librtemscpu_a_OBJECTS) ; do echo "$$i" >>librtemscpu.txt ; done
	$(librtemscpu_a_AR) librtemscpu.a @librtemscpu.txt $(librtemscpu_a_LIBADD)
	$(RANLIB) librtemscpu.a

endif
