#
#  COPYRIGHT (c) 1988-2012.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = cpu_supplement

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

REPLACE2 = $(PERL) $(top_srcdir)/tools/word-replace2

GENERATED_FILES =
GENERATED_FILES += general.texi
GENERATED_FILES += arm.texi
GENERATED_FILES += avr.texi
GENERATED_FILES += bfin.texi
GENERATED_FILES += h8300.texi
GENERATED_FILES += i386.texi
GENERATED_FILES += lm32.texi
GENERATED_FILES += m32c.texi
GENERATED_FILES += m32r.texi
GENERATED_FILES += m68k.texi
GENERATED_FILES += microblaze.texi
GENERATED_FILES += mips.texi
GENERATED_FILES += or1k.texi
GENERATED_FILES += powerpc.texi
GENERATED_FILES += nios2.texi
GENERATED_FILES += sh.texi
GENERATED_FILES += sparc.texi
GENERATED_FILES += sparc64.texi
GENERATED_FILES += v850.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES = preface.texi

info_TEXINFOS = cpu_supplement.texi
cpu_supplement_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

#
#  Chapters which get automatic processing
#

general.texi: general.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "" < $< > $@

arm.texi: arm.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

avr.texi: avr.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

bfin.texi: bfin.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

h8300.texi: h8300.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

i386.texi: i386.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

m32c.texi: m32c.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

m32r.texi: m32r.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

lm32.texi: lm32.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

m68k.texi: m68k.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

microblaze.texi: microblaze.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

mips.texi: mips.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

or1k.texi: or1k.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

powerpc.texi: powerpc.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

nios2.texi: nios2.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

sh.texi: sh.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

sparc.texi: sparc.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

sparc64.texi: sparc64.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

v850.texi: v850.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

CLEANFILES += cpu_supplement.info
CLEANFILES += cpu_supplement.info-1
CLEANFILES += cpu_supplement.info-2

