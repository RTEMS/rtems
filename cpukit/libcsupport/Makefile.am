##
##  $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libcsupport.a
libcsupport_a_CPPFLAGS = $(AM_CPPFLAGS)

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = include/chain.h include/console.h include/clockdrv.h \
    include/iosupp.h include/ringbuf.h include/rtc.h include/spurious.h \
    include/timerdrv.h include/vmeintr.h

## motorola

include_rtems_motoroladir = $(includedir)/rtems/motorola

include_rtems_motorola_HEADERS = include/motorola/mc68230.h \
    include/motorola/mc68681.h

## rtems
include_rtems_HEADERS += include/rtems/assoc.h include/rtems/error.h \
    include/rtems/libcsupport.h include/rtems/libio.h include/rtems/libio_.h \
    include/rtems/termiostypes.h

## zilog

include_rtems_zilogdir = $(includedir)/rtems/zilog

include_rtems_zilog_HEADERS = include/zilog/z8036.h include/zilog/z8530.h \
    include/zilog/z8536.h

##  General stuff
ERROR_C_FILES = src/error.c src/__assert.c

ASSOCIATION_C_FILES = src/assoc.c src/assoclocalbyname.c \
    src/assoclocalbyremotebitfield.c src/assoclocalbyremote.c \
    src/assocnamebad.c src/assocnamebylocalbitfield.c \
    src/assocnamebylocal.c src/assocnamebyremotebitfield.c \
    src/assocnamebyremote.c src/assocptrbylocal.c src/assocptrbyname.c \
    src/assocptrbyremote.c src/assocremotebylocalbitfield.c \
    src/assocremotebylocal.c src/assocremotebyname.c

BASE_FS_C_FILES = src/base_fs.c src/mount.c src/unmount.c src/libio.c \
    src/libio_init.c \
    src/libio_sockets.c src/eval.c src/fs_null_handlers.c src/privateenv.c

TERMIOS_C_FILES = src/cfgetispeed.c src/cfgetospeed.c src/cfsetispeed.c \
    src/cfsetospeed.c src/tcgetattr.c src/tcsetattr.c src/tcdrain.c \
    src/tcflow.c src/tcflush.c src/tcgetprgrp.c src/tcsendbreak.c \
    src/tcsetpgrp.c src/termios.c src/termiosinitialize.c \
    src/termiosreserveresources.c

SYSTEM_CALL_C_FILES = src/open.c src/close.c src/read.c src/write.c \
    src/write_r.c \
    src/lseek.c src/ioctl.c src/mkdir.c src/mknod.c src/mkfifo.c src/rmdir.c \
    src/chdir.c src/chmod.c src/fchdir.c src/fchmod.c src/chown.c src/link.c \
    src/unlink.c src/umask.c src/ftruncate.c src/utime.c src/fstat.c \
    src/fcntl.c src/fpathconf.c src/getdents.c src/fsync.c src/fdatasync.c \
    src/pipe.c src/dup.c src/dup2.c src/symlink.c src/readlink.c src/creat.c \
    src/chroot.c src/sync.c

DIRECTORY_SCAN_C_FILES = src/opendir.c src/closedir.c src/readdir.c \
    src/readdir_r.c src/rewinddir.c src/scandir.c src/seekdir.c \
    src/telldir.c src/getcwd.c

ID_C_FILES = src/getegid.c src/geteuid.c src/getgid.c src/getgroups.c \
    src/getlogin.c src/getpgrp.c src/getpid.c src/getppid.c src/getuid.c \
    src/setpgid.c src/setsid.c

MALLOC_C_FILES = src/malloc.c src/mallocfreespace.c src/__brk.c \
    src/__sbrk.c

PASSWORD_GROUP_C_FILES = src/getpwent.c

TERMINAL_IDENTIFICATION_C_FILES = src/ctermid.c src/isatty.c src/ttyname.c

LIBC_GLUE_C_FILES = src/__getpid.c src/__gettod.c src/__times.c \
    src/truncate.c src/access.c src/stat.c src/lstat.c src/pathconf.c \
    src/newlibc.c src/newlibc_exit.c src/no_posix.c src/no_libc.c src/utsname.c

BSD_LIBC_C_FILES = src/strlcpy.c src/strlcat.c

libcsupport_a_SOURCES = src/gxx_wrappers.c src/printk.c $(BSD_LIBC_C_FILES) \
    $(BASE_FS_C_FILES) $(MALLOC_C_FILES) $(ERROR_C_FILES) \
    $(ASSOCIATION_C_FILES)

if UNIX
libcsupport_a_SOURCES += src/unixlibc.c src/unixlibc_io.c src/hosterr.c
else
libcsupport_a_SOURCES += $(LIBC_GLUE_C_FILES) $(PASSWORD_GROUP_C_FILES) \
    $(TERMINAL_IDENTIFICATION_C_FILES) $(SYSTEM_CALL_C_FILES) \
    $(DIRECTORY_SCAN_C_FILES) $(ID_C_FILES) src/envlock.c \
    $(TERMIOS_C_FILES) src/getpagesize.c
endif

EXTRA_DIST = src/TODO src/CASES src/README

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
