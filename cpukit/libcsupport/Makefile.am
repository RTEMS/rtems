##
##  $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

include_HEADERS = include/chain.h include/console.h include/clockdrv.h \
    include/iosupp.h include/ringbuf.h include/rtc.h \
    include/spurious.h include/timerdrv.h include/vmeintr.h

if NEED_STDINT_H
LIBC_HFILES = include/stdint.h
endif
if NEED_INTTYPES_H
LIBC_HFILES += include/inttypes.h
endif
include_HEADERS += $(LIBC_HFILES)

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/motorola:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/zilog:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

## motorola

include_motoroladir = $(includedir)/motorola

include_motorola_HEADERS = include/motorola/mc68230.h include/motorola/mc68681.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola \
    $(include_motorola_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## rtems
include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = \
    include/rtems/assoc.h include/rtems/error.h include/rtems/libcsupport.h \
    include/rtems/libio.h include/rtems/libio_.h \
    include/rtems/termiostypes.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## sys

include_sysdir = $(includedir)/sys

if NEWLIB
NEWLIB_H_FILES = include/sys/termios.h include/sys/utsname.h
endif

if NEED_SYS_CDEFS_H
SYS_H_FILES = include/sys/cdefs.h
endif

# FIXME: We should not install to include/sys unless using newlib.
include_sys_HEADERS = \
include/sys/filio.h \
include/sys/ioccom.h \
include/sys/ioctl.h \
include/sys/sockio.h \
include/sys/termios.h \
include/sys/ttycom.h \
$(SYS_H_FILES) \
$(NEWLIB_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys \
    $(include_sys_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## zilog

include_zilogdir = $(includedir)/zilog

include_zilog_HEADERS = include/zilog/z8036.h include/zilog/z8530.h \
    include/zilog/z8536.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog \
    $(include_zilog_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

##  General stuff
all-local: $(PREINSTALL_FILES)

LIB = ${ARCH}/libcsupport.a

AM_CPPFLAGS += -I$(srcdir)/src

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
    src/getlogin.c src/getpgrp.c \
    src/getpid.c src/getppid.c src/getuid.c src/setpgid.c src/setsid.c

MALLOC_C_FILES = src/malloc.c src/mallocfreespace.c src/__brk.c src/__sbrk.c

PASSWORD_GROUP_C_FILES = src/getpwent.c

TERMINAL_IDENTIFICATION_C_FILES = src/ctermid.c src/isatty.c src/ttyname.c \
    src/ttyname_r.c

LIBC_GLUE_C_FILES = src/__getpid.c src/__gettod.c src/__times.c \
    src/truncate.c src/access.c src/stat.c src/lstat.c src/pathconf.c \
    src/newlibc.c src/no_posix.c src/no_libc.c src/utsname.c

UNIX_LIBC_C_FILES = src/unixlibc.c src/hosterr.c

BSD_LIBC_C_FILES = src/strlcpy.c src/strlcat.c

COMMON_C_FILES = src/gxx_wrappers.c src/printk.c \
    $(BSD_LIBC_C_FILES) \
    $(BASE_FS_C_FILES) \
    $(MALLOC_C_FILES) $(TERMIOS_C_FILES) $(ERROR_C_FILES) \
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

include $(top_srcdir)/automake/local.am
