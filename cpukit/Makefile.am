##
## $Id$
##

ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

SUBDIRS = . ada score rtems sapi posix itron
SUBDIRS += libcsupport libblock libfs
SUBDIRS += libnetworking librpc
SUBDIRS += libmisc
SUBDIRS += httpd
SUBDIRS += ftpd
SUBDIRS += wrapup

noinst_DATA = preinstall-stamp

preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
CLEANFILES = preinstall-stamp

include_HEADERS =

if DEPRECATED
include_HEADERS += backward/asm.h backward/chain.h backward/clockdrv.h \
    backward/confdefs.h backward/console.h backward/imfs.h backward/iosupp.h \
    backward/ringbuf.h backward/rtc.h backward/spurious.h \
    backward/timerdrv.h backward/vmeintr.h

include_motoroladir = $(includedir)/motorola
include_motorola_HEADERS = backward/motorola/mc68681.h \
    backward/motorola/mc68230.h

include_zilogdir = $(includedir)/zilog
include_zilog_HEADERS = backward/zilog/z8536.h backward/zilog/z8036.h \
    backward/zilog/z8030.h

if !UNIX
include_HEADERS += backward/dosfs.h
endif

if HAS_ITRON
include_HEADERS += backward/itron.h
endif
endif

if NEWLIB
include_sysdir = $(includedir)/sys
include_sys_HEADERS =

include_HEADERS += libcsupport/include/stdint.h
include_HEADERS += libcsupport/include/inttypes.h
include_HEADERS += libcsupport/include/tar.h

include_sys_HEADERS += libcsupport/include/sys/ioccom.h \
    libcsupport/include/sys/filio.h libcsupport/include/sys/ioctl.h \
    libcsupport/include/sys/sockio.h libcsupport/include/sys/ttycom.h \
    libcsupport/include/sys/termios.h libcsupport/include/sys/utsname.h \
    libcsupport/include/sys/cdefs.h
endif

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = include/rtems/bspIo.h include/rtems/userenv.h \
    include/rtems/fs.h include/rtems/pci.h include/rtems/stdint.h \
    include/rtems/concat.h

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if DEPRECATED
$(PROJECT_INCLUDE)/asm.h: backward/asm.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/asm.h

$(PROJECT_INCLUDE)/chain.h: backward/chain.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/chain.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/chain.h

$(PROJECT_INCLUDE)/clockdrv.h: backward/clockdrv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/clockdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/clockdrv.h

$(PROJECT_INCLUDE)/confdefs.h: backward/confdefs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/confdefs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/confdefs.h

$(PROJECT_INCLUDE)/console.h: backward/console.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/console.h

$(PROJECT_INCLUDE)/imfs.h: backward/imfs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/imfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/imfs.h

$(PROJECT_INCLUDE)/iosupp.h: backward/iosupp.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/iosupp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/iosupp.h

$(PROJECT_INCLUDE)/ringbuf.h: backward/ringbuf.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ringbuf.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ringbuf.h

$(PROJECT_INCLUDE)/rtc.h: backward/rtc.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtc.h

$(PROJECT_INCLUDE)/spurious.h: backward/spurious.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/spurious.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/spurious.h

$(PROJECT_INCLUDE)/timerdrv.h: backward/timerdrv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/timerdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/timerdrv.h

$(PROJECT_INCLUDE)/vmeintr.h: backward/vmeintr.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/vmeintr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/vmeintr.h

$(PROJECT_INCLUDE)/motorola/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/motorola
	@: > $(PROJECT_INCLUDE)/motorola/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/motorola/$(dirstamp)

$(PROJECT_INCLUDE)/motorola/mc68681.h: backward/motorola/mc68681.h $(PROJECT_INCLUDE)/motorola/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/motorola/mc68681.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola/mc68681.h

$(PROJECT_INCLUDE)/motorola/mc68230.h: backward/motorola/mc68230.h $(PROJECT_INCLUDE)/motorola/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/motorola/mc68230.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/motorola/mc68230.h

$(PROJECT_INCLUDE)/zilog/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/zilog
	@: > $(PROJECT_INCLUDE)/zilog/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/zilog/$(dirstamp)

$(PROJECT_INCLUDE)/zilog/z8536.h: backward/zilog/z8536.h $(PROJECT_INCLUDE)/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/zilog/z8536.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog/z8536.h

$(PROJECT_INCLUDE)/zilog/z8036.h: backward/zilog/z8036.h $(PROJECT_INCLUDE)/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/zilog/z8036.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog/z8036.h

$(PROJECT_INCLUDE)/zilog/z8030.h: backward/zilog/z8030.h $(PROJECT_INCLUDE)/zilog/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/zilog/z8030.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/zilog/z8030.h

if !UNIX
$(PROJECT_INCLUDE)/dosfs.h: backward/dosfs.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/dosfs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/dosfs.h
endif

if HAS_ITRON
$(PROJECT_INCLUDE)/itron.h: backward/itron.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itron.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itron.h
endif
endif

if NEWLIB
$(PROJECT_INCLUDE)/sys/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/sys
	@: > $(PROJECT_INCLUDE)/sys/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/sys/$(dirstamp)

$(PROJECT_INCLUDE)/stdint.h: libcsupport/include/stdint.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/stdint.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/stdint.h

$(PROJECT_INCLUDE)/inttypes.h: libcsupport/include/inttypes.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/inttypes.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/inttypes.h

$(PROJECT_INCLUDE)/tar.h: libcsupport/include/tar.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tar.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tar.h

$(PROJECT_INCLUDE)/sys/ioccom.h: libcsupport/include/sys/ioccom.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ioccom.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ioccom.h

$(PROJECT_INCLUDE)/sys/filio.h: libcsupport/include/sys/filio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/filio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/filio.h

$(PROJECT_INCLUDE)/sys/ioctl.h: libcsupport/include/sys/ioctl.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ioctl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ioctl.h

$(PROJECT_INCLUDE)/sys/sockio.h: libcsupport/include/sys/sockio.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/sockio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/sockio.h

$(PROJECT_INCLUDE)/sys/ttycom.h: libcsupport/include/sys/ttycom.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/ttycom.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/ttycom.h

$(PROJECT_INCLUDE)/sys/termios.h: libcsupport/include/sys/termios.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/termios.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/termios.h

$(PROJECT_INCLUDE)/sys/utsname.h: libcsupport/include/sys/utsname.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/utsname.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/utsname.h

$(PROJECT_INCLUDE)/sys/cdefs.h: libcsupport/include/sys/cdefs.h $(PROJECT_INCLUDE)/sys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sys/cdefs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sys/cdefs.h
endif

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/bspIo.h: include/rtems/bspIo.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/bspIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/bspIo.h

$(PROJECT_INCLUDE)/rtems/userenv.h: include/rtems/userenv.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/userenv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/userenv.h

$(PROJECT_INCLUDE)/rtems/fs.h: include/rtems/fs.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/fs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/fs.h

$(PROJECT_INCLUDE)/rtems/pci.h: include/rtems/pci.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/pci.h

$(PROJECT_INCLUDE)/rtems/stdint.h: include/rtems/stdint.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/stdint.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/stdint.h

$(PROJECT_INCLUDE)/rtems/concat.h: include/rtems/concat.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/concat.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/concat.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
