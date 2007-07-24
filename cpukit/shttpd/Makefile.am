include $(top_srcdir)/automake/compile.am

if LIBNETWORKING
include_shttpddir = $(includedir)/shttpd

project_lib_LIBRARIES = libshttpd.a
libshttpd_a_CPPFLAGS = $(AM_CPPFLAGS) -DHAVE_MD5

# HACK ALERT:
# prefix all non-public symbols with _shttp_
# FIXME: There must be something better than this
libshttpd_a_CPPFLAGS += -Dcheck_authorization=_shttpd_check_authorization
libshttpd_a_CPPFLAGS += -Ddecode_url_encoded_string=_shttpd_decode_url_encoded_string
libshttpd_a_CPPFLAGS += -Dedit_passwords=_shttpd_edit_passwords
libshttpd_a_CPPFLAGS += -Delog=_shttpd_elog
libshttpd_a_CPPFLAGS += -Dget_dir=_shttpd_get_dir
libshttpd_a_CPPFLAGS += -Dget_file=_shttpd_get_file
libshttpd_a_CPPFLAGS += -Dget_headers_len=_shttpd_get_headers_len
libshttpd_a_CPPFLAGS += -Dget_mime_type=_shttpd_get_mime_type
libshttpd_a_CPPFLAGS += -Dinit_from_argc_argv=_shttpd_init_from_argc_argv
libshttpd_a_CPPFLAGS += -Dis_authorized_for_put=_shttpd_is_authorized_for_put
libshttpd_a_CPPFLAGS += -Dis_registered_uri=_shttpd_is_registered_uri
libshttpd_a_CPPFLAGS += -Dlog_access=_shttpd_log_access
libshttpd_a_CPPFLAGS += -Dparse_headers=_shttpd_parse_headers
libshttpd_a_CPPFLAGS += -Dput_dir=_shttpd_put_dir
libshttpd_a_CPPFLAGS += -Dsend_authorization_request=_shttpd_send_authorization_request
libshttpd_a_CPPFLAGS += -Dsend_server_error=_shttpd_send_server_error
libshttpd_a_CPPFLAGS += -Dset_close_on_exec=_shttpd_set_close_on_exec
libshttpd_a_CPPFLAGS += -Dset_mime_types=_shttpd_set_mime_types
libshttpd_a_CPPFLAGS += -Dset_non_blocking_mode=_shttpd_set_non_blocking_mode
libshttpd_a_CPPFLAGS += -Dsetup_embedded_stream=_shttpd_setup_embedded_stream
libshttpd_a_CPPFLAGS += -Dstop_stream=_shttpd_stop_stream
libshttpd_a_CPPFLAGS += -Dusage=_shttpd_usage

libshttpd_a_CPPFLAGS += -Dknown_http_methods=_shttpd_known_http_methods
libshttpd_a_CPPFLAGS += -Doptions=_shttpd_options
libshttpd_a_CPPFLAGS += -Dio_file=_shttpd_io_file
libshttpd_a_CPPFLAGS += -Dio_socket=_shttpd_io_socket
libshttpd_a_CPPFLAGS += -Dio_embedded=_shttpd_io_embedded
libshttpd_a_CPPFLAGS += -Dio_dir=_shttpd_io_dir
libshttpd_a_CPPFLAGS += -Dio_cgi=_shttpd_io_cgi

libshttpd_a_CPPFLAGS += -Dcurrent_time=_shttpd_current_time
libshttpd_a_CPPFLAGS += -Dtz_offset=_shttpd_tz_offset

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
