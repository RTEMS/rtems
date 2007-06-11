include $(top_srcdir)/automake/compile.am

if NEWLIB
include_shttpddir = $(includedir)/shttpd

project_lib_LIBRARIES = libshttpd.a
libshttpd_a_CPPFLAGS = $(AM_CPPFLAGS)

SRCS=		string.c shttpd.c log.c auth.c md5.c \
		cgi.c mime_type.c config.c \
		io_file.c io_socket.c io_ssl.c io_emb.c io_dir.c io_cgi.c
HDRS=		defs.h llist.h shttpd.h std_includes.h io.h md5.h ssl.h \
		compat_unix.h compat_win32.h compat_rtems.h

libshttpd_a_SOURCES = $(SRCS) $(HDRS)
libshttpd_a_SOURCES += compat_rtems.c
include_shttpd_HEADERS = shttpd.h

# Possible flags: (in brackets are rough numbers for 'gcc -O2' on i386)
# -DHAVE_MD5		- use system md5 library (-2kb)
# -DNDEBUG		- strip off all debug code (-5kb)
# -D_DEBUG		- build debug version (very noisy) (+6kb)
# -DNO_CGI		- disable CGI support (-5kb)
# -DNO_SSL		- disable SSL functionality (-2kb)
# -DNO_AUTH		- disable authorization support (-4kb)
# -DNO_GUI		- Win32 only. Build console version, no GUI
# -DCONFIG=\"file\"	- use `file' as the default config file
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
