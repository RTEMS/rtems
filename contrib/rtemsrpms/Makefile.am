##
## $Id$
##

TARGETS=@TARGETS@
RTEMS_API=@RTEMS_API@
RPMDBPATH=@RPMDBPATH@

all:
	export PATH=$(prefix)/bin:$$PATH; \
	for t in $(TARGETS) ; \
	 do \
	  rpmbuild -v $(RPMDBPATH) -ba $(srcdir)/rtems.specs --target=$$t-rtems$(RTEMS_API) ; \
          if [ $$? -ne 0 ]; then \
           exit 1 ; \
          fi \
	 done
