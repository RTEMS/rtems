# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.3 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

prefix = /usr/local/rtems
exec_prefix = ${prefix}

libdir = ${exec_prefix}/lib

NORMAL_INSTALL = :

ADA = /usr/local/rtems/bin/i386-rtemself-gcc
ADAFLAGS = -O2 -gnata -gnatE -gnato -msoft-float -mno-fp-ret-in-387

VERSION = 0.1.3

privatelibdir = $(libdir)/adasockets

SOURCES = \
	sockets-constants.ads \
	sockets-link.ads \
	sockets-multicast.ads \
	sockets-multicast.adb \
	sockets-naming.ads \
	sockets-naming.adb \
	sockets-thin.ads \
	sockets-utils.ads \
	sockets-utils.adb \
	sockets.ads \
	sockets.adb

OBJECTS = \
	sockets-constants.o \
	sockets-link.o \
	sockets-multicast.o \
	sockets-naming.o \
	sockets-thin.o \
	sockets-utils.o \
	sockets.o

ALIS = $(OBJECTS:.o=.ali)

CLEANFILES = $(OBJECTS) $(ALIS)

.SUFFIXES:

all: $(OBJECTS) $(ALI)

install-exec: 
	@$(NORMAL_INSTALL)

install-data: install-data-local
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

mostlyclean-generic:
	-test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

mostlyclean:  mostlyclean-generic

clean:  clean-generic mostlyclean

.PHONY: tags distdir info dvi installcheck install-exec install-data \
install uninstall all installdirs mostlyclean-generic distclean-generic \
clean-generic maintainer-clean-generic clean mostlyclean distclean \
maintainer-clean


%.o %.ali: %.adb %.ads
	$(ADA) -c $(ADAFLAGS) $<

%.o %.ali: %.adb
	$(ADA) -c $(ADAFLAGS) $<

%.o %.ali: %.ads
	$(ADA) -c $(ADAFLAGS) $<


install-data-local: all
	rm -rf $(privatelibdir)
	mkdir $(privatelibdir)
	cp -p $(OBJECTS) $(ALIS) $(privatelibdir)
	for i in $(SOURCES); do \
	  if test -f $$i; then \
	    cp -p $$i $(privatelibdir); \
	  else \
	    cp -p $(srcdir)/$$i $(privatelibdir); \
	  fi \
	done


### DO NOT REMOVE THIS LINE, IT IS USED BY MAKEDEPEND ###
sockets-constants.o: sockets.ads sockets-constants.ads
sockets-link.o: sockets.ads sockets-link.ads
sockets-multicast.o: sockets.ads sockets.adb sockets-constants.ads sockets-link.ads sockets-multicast.ads sockets-multicast.adb sockets-naming.ads sockets-thin.ads sockets-utils.ads
sockets-naming.o: sockets.ads sockets-constants.ads sockets-naming.ads sockets-naming.adb sockets-thin.ads
sockets-thin.o: sockets.ads sockets-thin.ads
sockets-utils.o: sockets.ads sockets-utils.ads sockets-utils.adb
sockets.o: sockets.ads sockets.adb sockets-constants.ads sockets-link.ads sockets-naming.ads sockets-thin.ads sockets-utils.ads
split.o: split.adb

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
