##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

LIBNAME = libcsupport
LIB = ${ARCH}/${LIBNAME}.a

ERROR_C_FILES = error.c

ASSOCIATION_C_FILES = assoc.c assocnamebad.c

BASE_FS_C_FILES = base_fs.c mount.c unmount.c ioman.c libio.c \
    libio_sockets.c eval.c fs_null_handlers.c

IMFS_C_FILES = imfs_chown.c imfs_creat.c imfs_directory.c imfs_eval.c \
    imfs_free.c imfs_fsunmount.c imfs_gtkn.c imfs_init.c imfs_initsupp.c \
    imfs_link.c imfs_mknod.c imfs_mount.c imfs_fchmod.c imfs_unlink.c \
    imfs_unmount.c imfs_utime.c imfs_ntype.c imfs_stat.c imfs_getchild.c \
    memfile.c deviceio.c imfs_handlers_device.c imfs_handlers_directory.c \
    imfs_handlers_link.c imfs_handlers_memfile.c imfs_debug.c imfs_rmnod.c \
    imfs_symlink.c imfs_readlink.c imfs_fdatasync.c imfs_fcntl.c \
    miniimfs_init.c

TERMIOS_C_FILES = cfgetispeed.c cfgetospeed.c cfsetispeed.c cfsetospeed.c \
    tcgetattr.c tcsetattr.c tcdrain.c tcflow.c tcflush.c tcgetprgrp.c \
    tcsendbreak.c tcsetpgrp.c termios.c termiosinitialize.c \
    termiosreserveresources.c

SYSTEM_CALL_C_FILES = open.c close.c read.c write.c lseek.c ioctl.c mkdir.c \
    mknod.c mkfifo.c rmdir.c chdir.c chmod.c fchdir.c fchmod.c chown.c \
    link.c unlink.c umask.c ftruncate.c utime.c fstat.c fcntl.c fpathconf.c \
    getdents.c fsync.c fdatasync.c pipe.c dup.c dup2.c symlink.c readlink.c \
    creat.c

DIRECTORY_SCAN_C_FILES = opendir.c closedir.c readdir.c rewinddir.c \
    scandir.c seekdir.c telldir.c getcwd.c

MALLOC_C_FILES = malloc.c __brk.c __sbrk.c

PASSWORD_GROUP_C_FILES = getpwent.c getgrent.c

TERMINAL_IDENTIFICATION_C_FILES = ctermid.c isatty.c ttyname.c ttyname_r.c

LIBC_GLUE_C_FILES = __getpid.c __gettod.c __times.c truncate.c access.c \
    stat.c lstat.c pathconf.c newlibc.c no_posix.c no_libc.c

UNIX_LIBC_C_FILES = unixlibc.c hosterr.c

COMMON_C_FILES = gxx_wrappers.c printk.c $(BASE_FS_C_FILES) \
    $(MALLOC_C_FILES) $(TERMIOS_C_FILES) $(ERROR_C_FILES) \
    $(ASSOCIATION_C_FILES)

UNIX_C_FILES = $(UNIX_LIBC_C_FILES) imfs_unixstub.c

EMBEDDED_C_FILES = $(LIBC_GLUE_C_FILES) $(PASSWORD_GROUP_C_FILES) \
    $(TERMINAL_IDENTIFICATION_C_FILES) $(SYSTEM_CALL_C_FILES) \
    $(DIRECTORY_SCAN_C_FILES) $(IMFS_C_FILES)

if UNIX
C_FILES = $(COMMON_C_FILES) $(UNIX_C_FILES)
else
C_FILES = $(COMMON_C_FILES) $(EMBEDDED_C_FILES)
endif
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = libcsupport.h imfs.h
SYS_H_FILES =
RTEMS_H_FILES = assoc.h error.h libio.h
noinst_HEADERS = libio_.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems \
    $(PROJECT_INCLUDE)/sys $(H_FILES:%=$(PROJECT_INCLUDE)/%) \
    $(RTEMS_H_FILES:%=$(PROJECT_INCLUDE)/rtems/%) \
    $(SYS_H_FILES:%=$(PROJECT_INCLUDE)/sys/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/sys/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a

OBJS = $(C_O_FILES)

#
# Add local stuff here using +=
#

AM_CFLAGS += $(LIBC_DEFINES)

all-local: ${ARCH} $(TMPINSTALL_FILES)

$(LIB): ${OBJS}
	$(make-library)

$(PROJECT_RELEASE)/lib/$(LIBNAME)$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

DOC_FILES = TODO CASES

EXTRA_DIST = $(DOC_FILES) $(COMMON_C_FILES) $(EMBEDDED_C_FILES) \
    $(UNIX_C_FILES) $(H_FILES) $(RTEMS_H_FILES) $(SYS_H_FILES)

include $(top_srcdir)/../../../automake/local.am
