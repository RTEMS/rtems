#
#  $Id$
#

include Make.config

BASEDIR=$(shell pwd)

all: env info html ps
#	find $(WWW_INSTALL) -type f | xargs -e chmod 444
#	find $(WWW_INSTALL) -type d | xargs -e chmod 555

env: tools dirs headers

dirs:
	$(make-dirs)

headers:
	sed -e 's/RTEMS_VERSION/$(RTEMS_VERSION)/' \
	    <$(SRCDIR)/index.html >$(WWW_INSTALL)/index.html
	sed -e 's/RTEMS_VERSION/$(RTEMS_VERSION)/' \
	    <$(SRCDIR)/rtems_support.html >$(WWW_INSTALL)/rtems_support.html
	sed -e 's/RTEMS_VERSION/$(RTEMS_VERSION)/' \
	    <$(SRCDIR)/rtems_header.html >$(WWW_INSTALL)/rtems_header.html
	sed -e 's/RTEMS_VERSION/$(RTEMS_VERSION)/' \
	    <$(SRCDIR)/rtems_footer.html >$(WWW_INSTALL)/rtems_footer.html

info: env
	./do_docs $(BASEDIR) info

html: env
	./do_docs $(BASEDIR) html

ps: dirs env
	./do_docs $(BASEDIR) ps

clean:
	./do_docs $(BASEDIR) clean
	cd tools/bmenu ; gmake clean
	cd tools/pdl2texi ; gmake clean
	cd tools/src2html ; gmake clean


.PHONY: tools

tools:
	cd tools/bmenu ; gmake 
	cd tools/pdl2texi ; gmake 
	cd tools/src2html ; gmake
