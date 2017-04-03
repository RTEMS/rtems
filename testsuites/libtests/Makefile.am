ACLOCAL_AMFLAGS = -I ../aclocal

_SUBDIRS  =
_SUBDIRS += POSIX
_SUBDIRS += block01
_SUBDIRS += block02
_SUBDIRS += block03
_SUBDIRS += block04
_SUBDIRS += block05
_SUBDIRS += block06
_SUBDIRS += block07
_SUBDIRS += block08
_SUBDIRS += block09
_SUBDIRS += block10
_SUBDIRS += block11
_SUBDIRS += block12
_SUBDIRS += block13
_SUBDIRS += block14
_SUBDIRS += block15
_SUBDIRS += block16
_SUBDIRS += block17
_SUBDIRS += bspcmdline01
_SUBDIRS += capture01
_SUBDIRS += complex
_SUBDIRS += cpuuse
_SUBDIRS += crypt01
_SUBDIRS += defaultconfig01
_SUBDIRS += devfs01
_SUBDIRS += devfs02
_SUBDIRS += devfs03
_SUBDIRS += devfs04
_SUBDIRS += deviceio01
_SUBDIRS += devnullfatal01
_SUBDIRS += dumpbuf01
_SUBDIRS += exit01
_SUBDIRS += exit02
_SUBDIRS += flashdisk01
_SUBDIRS += gxx01
_SUBDIRS += heapwalk
_SUBDIRS += i2c01
_SUBDIRS += libfdt01
_SUBDIRS += malloc02
_SUBDIRS += malloc03
_SUBDIRS += malloc04
_SUBDIRS += malloctest
_SUBDIRS += math
_SUBDIRS += mathf
_SUBDIRS += mathl
_SUBDIRS += md501
_SUBDIRS += monitor
_SUBDIRS += monitor02
_SUBDIRS += mouse01
_SUBDIRS += newlib01
_SUBDIRS += putenvtest
_SUBDIRS += pwdgrp01
_SUBDIRS += pwdgrp02
_SUBDIRS += rbheap01
_SUBDIRS += rtmonuse
_SUBDIRS += sha
_SUBDIRS += shell01
_SUBDIRS += sparsedisk01
_SUBDIRS += spi01
_SUBDIRS += stackchk
_SUBDIRS += stackchk01
_SUBDIRS += stringto01
_SUBDIRS += tar01
_SUBDIRS += tar02
_SUBDIRS += tar03
_SUBDIRS += termios
_SUBDIRS += termios01
_SUBDIRS += termios02
_SUBDIRS += termios03
_SUBDIRS += termios04
_SUBDIRS += termios05
_SUBDIRS += termios06
_SUBDIRS += termios07
_SUBDIRS += termios08
_SUBDIRS += termios09
_SUBDIRS += top
_SUBDIRS += tztest
_SUBDIRS += uid01
_SUBDIRS += utf8proc01

if NETTESTS
if HAS_POSIX
_SUBDIRS += mghttpd01
endif
_SUBDIRS += ftp01
_SUBDIRS += networking01
_SUBDIRS += syscall01
endif

##
## The libdl use use AM's BUILT_SOURCE and this has a weird side effect of
## restarting the subdir loop. No idea why.
##
if DLTESTS
_SUBDIRS += dl01 dl02 dl03
if HAS_CXX
_SUBDIRS += dl04 dl05
endif
endif

if DEBUGGERTESTS
_SUBDIRS += debugger01
endif

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
