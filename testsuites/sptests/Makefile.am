##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

## Test names are spaced to indicate gaps in numbering from tests being
## deleted over the years.
SUBDIRS = \
    sp01 sp02 sp03 sp04 sp05 sp06 sp07 sp08 sp09 \
    sp10 sp11 sp12 sp13 sp14 sp15 sp16 sp17 sp18 sp19 \
    sp20 sp21 sp22 sp23 sp24 sp25 sp26 sp27 sp27a sp28 sp29 \
    sp30 sp31 sp32 sp33 sp34 sp35      sp37 sp38 sp39 \
    sp40 sp41 sp42 sp43 sp44 sp45 sp46 sp47 sp48 sp49 \
    sp50 sp51 sp52 sp53 sp54 sp55 sp56 sp57 sp58 sp59 \
    sp60 sp61 sp62 sp63 sp64 \
    spchain spobjgetnext spprintk spsize spstkalloc spthreadq01 \
    spwatchdog spwkspace \
    spfatal01 spfatal02 spfatal03 spfatal04 spfatal05 spfatal06 spfatal07 \
    spfatal08 spfatal09 spfatal10 spfatal11 spfatal12 \
    spintrcritical01 spintrcritical02 spintrcritical03 spintrcritical04 \
    spintrcritical05 spintrcritical06 spintrcritical07 spintrcritical08 \
    spintrcritical09 spintrcritical10 spintrcritical11 spintrcritical12 \
    spintrcritical13 spintrcritical14 spintrcritical15 spintrcritical16
 
DIST_SUBDIRS = $(SUBDIRS) spfatal_support spintrcritical_support
EXTRA_DIST = spfatal_support/init.c spfatal_support/system.h

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
