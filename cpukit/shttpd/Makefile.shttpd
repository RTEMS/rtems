SRCS=		string.c shttpd.c log.c auth.c md5.c \
		cgi.c mime_type.c config.c \
		io_file.c io_socket.c io_ssl.c io_emb.c io_dir.c io_cgi.c
HDRS=		defs.h llist.h shttpd.h std_includes.h io.h md5.h ssl.h \
		compat_unix.h compat_win32.h compat_rtems.h
PROG=		shttpd

# Possible flags: (in brackets are rough numbers for 'gcc -O2' on i386)
# -DHAVE_MD5		- use system md5 library (-2kb)
# -DNDEBUG		- strip off all debug code (-5kb)
# -D_DEBUG		- build debug version (very noisy) (+6kb)
# -DNO_CGI		- disable CGI support (-5kb)
# -DNO_SSL		- disable SSL functionality (-2kb)
# -DNO_AUTH		- disable authorization support (-4kb)
# -DNO_GUI		- Win32 only. Build console version, no GUI
# -DCONFIG=\"file\"	- use `file' as the default config file

CL_FLAGS=	/O1 /MD /TC /nologo /DNDEBUG

all:
	@echo "make (unix|msvc|mingw|rtems)"

unix:
	$(CC) -c $(CFLAGS) -DEMBEDDED $(SRCS) compat_unix.c
	$(AR) -r lib$(PROG).a *.o && ranlib lib$(PROG).a 
	$(CC) $(CFLAGS) $(SRCS) compat_unix.c standalone.c -o $(PROG) $(LIBS)

rtems:
	$(CC) -c $(CFLAGS) -DEMBEDDED $(SRCS) compat_rtems.c
	$(AR) -r lib$(PROG).a *.o && ranlib lib$(PROG).a 

msvc:
	cl $(SRCS) /c $(CL_FLAGS) /DEMBEDDED
	lib *.obj /out:shttpd.lib
	rc resources.rc
	cl $(SRCS) compat_win32.c standalone.c  $(CL_FLAGS) \
		/link resources.res /out:$(PROG).exe /machine:ix86

mingw:
	$(CC) -c $(CFLAGS) -DEMBEDDED $(SRCS) compat_win32.c
	$(AR) -r lib$(PROG).a *.o && ranlib lib$(PROG).a 
	windres resources.rc resources.o
	$(CC) $(CFLAGS) $(SRCS) compat_win32.c standalone.c resources.o -o $(PROG) $(LIBS) -lws2_32 -lcomdlg32 -lcomctl32

man:
	cat shttpd.1 | tbl | groff -man -Tascii | col -b > shttpd.1.txt
	cat shttpd.1 | tbl | groff -man -Tascii | less

clean:
	rm -rf *.o *.core $(PROG) lib$(PROG).a
