##
##  $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

include_HEADERS = include/chain.h include/console.h include/clockdrv.h \
    include/iosupp.h include/ringbuf.h include/rtc.h include/spurious.h \
    include/timerdrv.h include/vmeintr.h

if NEED_STDINT_H
include_HEADERS += include/stdint.h
endif
if NEED_INTTYPES_H
include_HEADERS += include/inttypes.h
endif

## motorola

include_motoroladir = $(includedir)/motorola

include_motorola_HEADERS = include/motorola/mc68230.h \
    include/motorola/mc68681.h

## rtems
include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/assoc.h include/rtems/error.h \
    include/rtems/libcsupport.h include/rtems/libio.h include/rtems/libio_.h \
    include/rtems/termiostypes.h include/rtems/cdefs.h

## sys

include_sysdir = $(includedir)/sys

# FIXME: We should not install to include/sys unless using newlib.
include_sys_HEADERS = include/sys/filio.h include/sys/ioccom.h \
    include/sys/ioctl.h include/sys/sockio.h include/sys/ttycom.h

if NEWLIB
include_sys_HEADERS += include/sys/termios.h include/sys/utsname.h
endif

if NEED_SYS_CDEFS_H
include_sys_HEADERS += include/sys/cdefs.h
endif

## zilog

include_zilogdir = $(includedir)/zilog

include_zilog_HEADERS = include/zilog/z8036.h include/zilog/z8530.h \
    include/zilog/z8536.h

##  General stuff
all-local: $(PREINSTALL_FILES)

LIB = ${ARCH}/libcsupport.a

AM_CPPFLAGS += -I$(top_builddir)

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

UNIX_LIBC_C_FILES = src/unixlibc.c src/hosterr.c

BSD_LIBC_C_FILES = src/strlcpy.c src/strlcat.c

COMMON_C_FILES = src/gxx_wrappers.c src/printk.c $(BSD_LIBC_C_FILES) \
    $(BASE_FS_C_FILES) $(MALLOC_C_FILES) $(TERMIOS_C_FILES) $(ERROR_C_FILES) \
    $(ASSOCIATION_C_FILES)

UNIX_C_FILES = $(UNIX_LIBC_C_FILES)

EMBEDDED_C_FILES = $(LIBC_GLUE_C_FILES) $(PASSWORD_GROUP_C_FILES) \
    $(TERMINAL_IDENTIFICATION_C_FILES) $(SYSTEM_CALL_C_FILES) \
    $(DIRECTORY_SCAN_C_FILES) $(ID_C_FILES)

if UNIX
C_FILES = $(COMMON_C_FILES) $(UNIX_C_FILES)
else
C_FILES = $(COMMON_C_FILES) $(EMBEDDED_C_FILES)
endif

OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

#
# Add local stuff here using +=
#

AM_CFLAGS += $(LIBC_DEFINES)

all-local: ${ARCH} ${LIB}

${ARCH}/%.$(OBJEXT): src/%.c
	${COMPILE} -o $@ -c $<

$(LIB): ${OBJS}
	$(make-library)

DOC_FILES = src/TODO src/CASES

EXTRA_DIST = $(DOC_FILES) $(COMMON_C_FILES) $(EMBEDDED_C_FILES) \
    $(UNIX_C_FILES)

BUILT_SOURCES = $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/chain.h: include/chain.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/chain.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/chain.h

$(PROJECT_INCLUDE)/console.h: include/console.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/console.h

$(PROJECT_INCLUDE)/clockdrv.h: include/clockdrv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/clockdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/clockdrv.h

$(PROJECT_INCLUDE)/iosupp.h: include/iosupp.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/iosupp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/iosupp.h

$(PROJECT_INCLUDE)/ringbuf.h: include/ringbuf.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ringbuf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ringbuf.h

$(PROJECT_INCLUDE)/rtc.h: include/rtc.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtc.h

$(PROJECT_INCLUDE)/spurious.h: include/spurious.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/spurious.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/spurious.h

$(PROJECT_INCLUDE)/timerdrv.h: include/timerdrv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/timerdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/timerdrv.h

$(PROJECT_INCLUDE)/vmeintr.h: include/vmeintr.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/vmeintr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vmeintr.h

if NEED_STDINT_H
$(PROJECT_INCLUDE)/stdint.h: include/stdint.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/stdint.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/stdint.h
endif

if NEED_INTTYPES_H
$(PROJECT_INCLUDE)/inttypes.h: include/inttypes.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/inttypes.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/inttypes.h
endif

$(PROJECT_INCLUDE)/motorola/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/motorola
	@: > $(PROJECT_INCLUDE)/motorola/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola/$(dirstamp)

$(PROJECT_INCLUDE)/motorola/mc68230.h: include/motorola/mc68230.h $(PROJECT_INCLUDE)/motorola/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/motorola/mc68230.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola/mc68230.h

$(PROJECT_INCLUDE)/motorola/mc68681.h: include/motorola/mc68681.h $(PROJECT_INCLUDE)/motorola/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/motorola/mc68681.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola/mc68681.h

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

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

$(PROJECT_INCLUDE)/sys/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/sys
	@: > $(PROJECT_INCLUDE)/sys/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/$(dirstamp)

$(PROJECT_INCLUDE)/sys/filio.h: include/sys/filio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/filio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/filio.h

$(PROJECT_INCLUDE)/sys/ioccom.h: include/sys/ioccom.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ioccom.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ioccom.h

$(PROJECT_INCLUDE)/sys/ioctl.h: include/sys/ioctl.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ioctl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ioctl.h

$(PROJECT_INCLUDE)/sys/sockio.h: include/sys/sockio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/sockio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/sockio.h

$(PROJECT_INCLUDE)/sys/ttycom.h: include/sys/ttycom.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ttycom.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ttycom.h

if NEWLIB
$(PROJECT_INCLUDE)/sys/termios.h: include/sys/termios.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/termios.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/termios.h

$(PROJECT_INCLUDE)/sys/utsname.h: include/sys/utsname.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/utsname.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/utsname.h
endif

if NEED_SYS_CDEFS_H
$(PROJECT_INCLUDE)/sys/cdefs.h: include/sys/cdefs.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/cdefs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/cdefs.h
endif

$(PROJECT_INCLUDE)/zilog/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/zilog
	@: > $(PROJECT_INCLUDE)/zilog/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog/$(dirstamp)

$(PROJECT_INCLUDE)/zilog/z8036.h: include/zilog/z8036.h $(PROJECT_INCLUDE)/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/zilog/z8036.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog/z8036.h

$(PROJECT_INCLUDE)/zilog/z8530.h: include/zilog/z8530.h $(PROJECT_INCLUDE)/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/zilog/z8530.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog/z8530.h

$(PROJECT_INCLUDE)/zilog/z8536.h: include/zilog/z8536.h $(PROJECT_INCLUDE)/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/zilog/z8536.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog/z8536.h

include $(top_srcdir)/automake/local.am
