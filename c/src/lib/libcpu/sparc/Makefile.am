ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

noinst_PROGRAMS += syscall.rel
syscall_rel_SOURCES = syscall/syscall.S syscall/syscall.h
syscall_rel_CPPFLAGS = $(AM_CPPFLAGS)
syscall_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += reg_win.rel
reg_win_rel_SOURCES = reg_win/window.S
reg_win_rel_CPPFLAGS = $(AM_CPPFLAGS)
reg_win_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += access.rel
access_rel_SOURCES = access/access.S access/access_le.c
access_rel_CPPFLAGS = $(AM_CPPFLAGS)
access_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include $(top_srcdir)/../../../automake/local.am
