
include Make.config

BASEDIR=$(shell pwd)

all: info html ps
#	find $(WWW_INSTALL) -type f | xargs -e chmod 444
#	find $(WWW_INSTALL) -type d | xargs -e chmod 555

dirs:
	-mkdir $(WWW_INSTALL)
	-mkdir $(WWW_INSTALL)/info
	-mkdir $(WWW_INSTALL)/html
	-mkdir $(WWW_INSTALL)/ps

info: dirs
	./do_docs $(BASEDIR) info

html: dirs
	cp common/*.gif common/*.jpg $(WWW_INSTALL)
	cp rtems.html HELP.html $(WWW_INSTALL)
	./do_docs $(BASEDIR) html

ps: dirs
	./do_docs $(BASEDIR) ps

clean:
	./do_docs $(BASEDIR) clean


