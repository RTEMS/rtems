##
##  $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -I$(top_builddir)

EXTRA_LIBRARIES = libcsupport.a
CLEANFILES = libcsupport.a
libcsupport_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libcsupport_g.a
CLEANFILES += libcsupport_g.a
libcsupport_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = include/chain.h include/console.h include/clockdrv.h \
    include/iosupp.h include/ringbuf.h include/rtc.h include/spurious.h \
    include/timerdrv.h include/vmeintr.h

include_HEADERS =

#if NEWLIB
include_sysdir = $(includedir)/sys
include_sys_HEADERS =

if RTEMS_PROVIDES_STDINT_H
include_HEADERS += include/stdint.h
endif

if RTEMS_PROVIDES_INTTYPES_H
include_HEADERS += include/inttypes.h
endif
#endif

## motorola

include_rtems_motoroladir = $(includedir)/rtems/motorola

include_rtems_motorola_HEADERS = include/motorola/mc68230.h \
    include/motorola/mc68681.h

## rtems
include_rtems_HEADERS += include/rtems/assoc.h include/rtems/error.h \
    include/rtems/libcsupport.h include/rtems/libio.h include/rtems/libio_.h \
    include/rtems/termiostypes.h include/rtems/cdefs.h

## sys

if NEWLIB
include_sys_HEADERS += include/sys/ioccom.h include/sys/filio.h \
    include/sys/ioctl.h include/sys/sockio.h include/sys/ttycom.h \
    include/sys/termios.h include/sys/utsname.h include/sys/cdefs.h
endif

## zilog

include_rtems_zilogdir = $(includedir)/rtems/zilog

include_rtems_zilog_HEADERS = include/zilog/z8036.h include/zilog/z8530.h \
    include/zilog/z8536.h

##  General stuff
all-local: $(PREINSTALL_FILES) libcsupport$(LIB_VARIANT).a

ERROR_C_FILES = src/error.c

ASSOCIATION_C_FILES = src/assoc.c src/assocnamebad.c

BASE_FS_C_FILES = src/base_fs.c src/mount.c src/unmount.c src/libio.c \
    src/libio_sockets.c src/eval.c src/fs_null_handlers.c src/privateenv.c

TERMIOS_C_FILES = src/cfgetispeed.c src/cfgetospeed.c src/cfsetispeed.c \
    src/cfsetospeed.c src/tcgetattr.c src/tcsetattr.c src/tcdrain.c \
    src/tcflow.c src/tcflush.c src/tcgetprgrp.c src/tcsendbreak.c \
    src/tcsetpgrp.c src/termios.c src/termiosinitialize.c \
    src/termiosreserveresources.c

SYSTEM_CALL_C_FILES = src/open.c src/close.c src/read.c src/write.c \
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

TERMINAL_IDENTIFICATION_C_FILES = src/ctermid.c src/isatty.c src/ttyname.c \
    src/ttyname_r.c

LIBC_GLUE_C_FILES = src/__getpid.c src/__gettod.c src/__times.c \
    src/truncate.c src/access.c src/stat.c src/lstat.c src/pathconf.c \
    src/newlibc.c src/no_posix.c src/no_libc.c src/utsname.c

BSD_LIBC_C_FILES = src/strlcpy.c src/strlcat.c

libcsupport_a_SOURCES = src/gxx_wrappers.c src/printk.c $(BSD_LIBC_C_FILES) \
    $(BASE_FS_C_FILES) $(MALLOC_C_FILES) $(ERROR_C_FILES) \
    $(ASSOCIATION_C_FILES)

if UNIX
libcsupport_a_SOURCES += src/unixlibc.c src/hosterr.c
else
libcsupport_a_SOURCES += $(LIBC_GLUE_C_FILES) $(PASSWORD_GROUP_C_FILES) \
    $(TERMINAL_IDENTIFICATION_C_FILES) $(SYSTEM_CALL_C_FILES) \
    $(DIRECTORY_SCAN_C_FILES) $(ID_C_FILES) src/envlock.c \
    $(TERMIOS_C_FILES)
endif

libcsupport_g_a_SOURCES = $(libcsupport_a_SOURCES)

EXTRA_DIST = src/TODO src/CASES src/README

DISTCLEANFILES =

libcsupport/include/sys/$(dirstamp):
	@$(mkdir_p) libcsupport/include/sys
	@: > libcsupport/include/sys/$(dirstamp)

if NEWLIB
libcsupport/include/sys/ioccom.h: include/sys/ioccom.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/ioccom.h

libcsupport/include/sys/filio.h: include/sys/filio.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/filio.h

libcsupport/include/sys/ioctl.h: include/sys/ioctl.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/ioctl.h

libcsupport/include/sys/sockio.h: include/sys/sockio.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/sockio.h

libcsupport/include/sys/ttycom.h: include/sys/ttycom.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/ttycom.h

libcsupport/include/sys/termios.h: include/sys/termios.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/termios.h

libcsupport/include/sys/utsname.h: include/sys/utsname.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/utsname.h

libcsupport/include/sys/cdefs.h: include/sys/cdefs.h libcsupport/include/sys/$(dirstamp)
	$(INSTALL_DATA) $< libcsupport/include/sys/cdefs.h
endif

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/chain.h: include/chain.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/chain.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/chain.h

$(PROJECT_INCLUDE)/rtems/console.h: include/console.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/console.h

$(PROJECT_INCLUDE)/rtems/clockdrv.h: include/clockdrv.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/clockdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/clockdrv.h

$(PROJECT_INCLUDE)/rtems/iosupp.h: include/iosupp.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/iosupp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/iosupp.h

$(PROJECT_INCLUDE)/rtems/ringbuf.h: include/ringbuf.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/ringbuf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ringbuf.h

$(PROJECT_INCLUDE)/rtems/rtc.h: include/rtc.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/rtc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtc.h

$(PROJECT_INCLUDE)/rtems/spurious.h: include/spurious.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/spurious.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/spurious.h

$(PROJECT_INCLUDE)/rtems/timerdrv.h: include/timerdrv.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/timerdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/timerdrv.h

$(PROJECT_INCLUDE)/rtems/vmeintr.h: include/vmeintr.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/vmeintr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/vmeintr.h

$(PROJECT_INCLUDE)/sys/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/sys
	@: > $(PROJECT_INCLUDE)/sys/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/sys/$(dirstamp)

if RTEMS_PROVIDES_STDINT_H
$(PROJECT_INCLUDE)/stdint.h: include/stdint.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/stdint.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/stdint.h
endif

if RTEMS_PROVIDES_INTTYPES_H
$(PROJECT_INCLUDE)/inttypes.h: include/inttypes.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/inttypes.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/inttypes.h
endif

$(PROJECT_INCLUDE)/rtems/motorola/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/motorola
	@: > $(PROJECT_INCLUDE)/rtems/motorola/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/motorola/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/motorola/mc68230.h: include/motorola/mc68230.h $(PROJECT_INCLUDE)/rtems/motorola/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/motorola/mc68230.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/motorola/mc68230.h

$(PROJECT_INCLUDE)/rtems/motorola/mc68681.h: include/motorola/mc68681.h $(PROJECT_INCLUDE)/rtems/motorola/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/motorola/mc68681.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/motorola/mc68681.h

$(PROJECT_INCLUDE)/rtems/assoc.h: include/rtems/assoc.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/assoc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/assoc.h

$(PROJECT_INCLUDE)/rtems/error.h: include/rtems/error.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/error.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/error.h

$(PROJECT_INCLUDE)/rtems/libcsupport.h: include/rtems/libcsupport.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/libcsupport.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/libcsupport.h

$(PROJECT_INCLUDE)/rtems/libio.h: include/rtems/libio.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/libio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/libio.h

$(PROJECT_INCLUDE)/rtems/libio_.h: include/rtems/libio_.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/libio_.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/libio_.h

$(PROJECT_INCLUDE)/rtems/termiostypes.h: include/rtems/termiostypes.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/termiostypes.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/termiostypes.h

$(PROJECT_INCLUDE)/rtems/cdefs.h: include/rtems/cdefs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/cdefs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/cdefs.h

if NEWLIB
$(PROJECT_INCLUDE)/sys/ioccom.h: include/sys/ioccom.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ioccom.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ioccom.h

$(PROJECT_INCLUDE)/sys/filio.h: include/sys/filio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/filio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/filio.h

$(PROJECT_INCLUDE)/sys/ioctl.h: include/sys/ioctl.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ioctl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ioctl.h

$(PROJECT_INCLUDE)/sys/sockio.h: include/sys/sockio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/sockio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/sockio.h

$(PROJECT_INCLUDE)/sys/ttycom.h: include/sys/ttycom.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ttycom.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ttycom.h

$(PROJECT_INCLUDE)/sys/termios.h: include/sys/termios.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/termios.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/termios.h

$(PROJECT_INCLUDE)/sys/utsname.h: include/sys/utsname.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/utsname.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/utsname.h

$(PROJECT_INCLUDE)/sys/cdefs.h: include/sys/cdefs.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/cdefs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/cdefs.h
endif

$(PROJECT_INCLUDE)/rtems/zilog/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/zilog
	@: > $(PROJECT_INCLUDE)/rtems/zilog/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/zilog/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/zilog/z8036.h: include/zilog/z8036.h $(PROJECT_INCLUDE)/rtems/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/zilog/z8036.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/zilog/z8036.h

$(PROJECT_INCLUDE)/rtems/zilog/z8530.h: include/zilog/z8530.h $(PROJECT_INCLUDE)/rtems/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/zilog/z8530.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/zilog/z8530.h

$(PROJECT_INCLUDE)/rtems/zilog/z8536.h: include/zilog/z8536.h $(PROJECT_INCLUDE)/rtems/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/zilog/z8536.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/zilog/z8536.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
