/*
 * bat.c
 *
 *	    This file contains the implementation of C function to
 *          Instantiate 60x/7xx ppc Block Address Translation (BAT) registers.
 *	    More detailed information can be found on motorola
 *	    site and more precisely in the following book :
 *
 *		MPC750
 *		Risc Microporcessor User's Manual
 *		Mtorola REF : MPC750UM/AD 8/97
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#include <rtems.h>
#include <libcpu/bat.h>
#include <libcpu/spr.h>
#include <rtems/bspIo.h>

#include <libcpu/cpuIdent.h>

#define TYP_I	1
#define TYP_D	0

typedef union
{                               /* BAT register values to be loaded */
  BAT bat;
  struct
  {
    unsigned int u, l;
  } words;
} ubat;

typedef struct batrange
{                               /* stores address ranges mapped by BATs */
  unsigned long start;
  unsigned long limit;
  unsigned long phys;
} batrange;

batrange bat_addrs[2][8] = { { {0,} } };

/* could encode this in bat_addrs but I don't touch that one for bwds compat. reasons */
/* bitmask of used bats */
static unsigned bat_in_use[2] = { 0, 0 };

/* define a few macros */

#define CLRBAT_ASM(batu,r)			\
	"	sync                 \n"	\
	"	isync                \n"	\
	"	li      "#r",    0   \n"	\
	"	mtspr	"#batu", "#r"\n"	\
	"	sync                 \n"	\
	"	isync                \n"

#define SETBAT_ASM(batu, batl, u, l)\
	"	mtspr	"#batl", "#l" \n"	\
	"	sync                  \n"	\
	"	isync                 \n"	\
	"	mtspr	"#batu", "#u" \n"	\
	"	sync                  \n"	\
	"	isync                 \n"

#define	CLRBAT(bat)					\
	asm volatile(					\
		CLRBAT_ASM(%0, 0)			\
		:							\
		:"i"(bat##U)				\
		:"0")

#define GETBAT(bat,u,l)				\
	asm volatile(					\
		"	mfspr %0, %2     \n"	\
		"	mfspr %1, %3     \n"	\
		:"=r"(u),"=r"(l)			\
		:"i"(bat##U),"i"(bat##L)	\
		)

#define DECL_SETBAT(lcbat,bat)		\
void								\
asm_set##lcbat(unsigned int upper, unsigned int lower)	\
{									\
asm volatile(						\
	CLRBAT_ASM(%0,0)				\
	SETBAT_ASM(%0,%1,%2,%3)			\
	:								\
	:"i"(bat##U),					\
	 "i"(bat##L),					\
	 "r"(upper),"r"(lower)			\
	:"0");							\
}

/* export the 'asm' versions for historic reasons */
DECL_SETBAT (dbat0, DBAT0)
DECL_SETBAT (dbat1, DBAT1)
DECL_SETBAT (dbat2, DBAT2)
DECL_SETBAT (dbat3, DBAT3)

static DECL_SETBAT (dbat4, DBAT4)
static DECL_SETBAT (dbat5, DBAT5)
static DECL_SETBAT (dbat6, DBAT6)
static DECL_SETBAT (dbat7, DBAT7)

static DECL_SETBAT (ibat0, IBAT0)
static DECL_SETBAT (ibat1, IBAT1)
static DECL_SETBAT (ibat2, IBAT2)
static DECL_SETBAT (ibat3, IBAT3)
static DECL_SETBAT (ibat4, IBAT4)
static DECL_SETBAT (ibat5, IBAT5)
static DECL_SETBAT (ibat6, IBAT6)
static DECL_SETBAT (ibat7, IBAT7)


SPR_RO (HID0);

static void
set_hid0_sync (unsigned long val)
{
  __asm__ volatile (
    "	sync			\n"
    "	isync			\n"
    "	mtspr	%0, %1	\n"
    "	sync			\n"
    "	isync			\n"
    :
    :"i" (HID0), "r" (val)
    :"memory" /* paranoia */
  );
}

static void
bat_addrs_put (ubat * bat, int typ, int idx)
{
  unsigned long bl;
  if (bat->bat.batu.vp || bat->bat.batu.vs) {
    bat_addrs[typ][idx].start = bat->bat.batu.bepi << 17;
    bat_addrs[typ][idx].phys = bat->bat.batl.brpn << 17;

    /* extended BL cannot be extracted using BAT union
     * - let's just hope the upper bits read 0 on pre 745x
     * CPUs.
     */
    bl = (bat->words.u << 15) | ((1 << 17) - 1);
    bat_addrs[typ][idx].limit = bat_addrs[typ][idx].start + bl;

    bat_in_use[typ] |= (1 << idx);
  }
}

/* We don't know how the board was initialized. Therefore,
 * when 'setdbat' is first used we must initialize our
 * cache.
 */
static void
bat_addrs_init (void)
{
  ubat bat;

  GETBAT (DBAT0, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_D, 0);
  GETBAT (DBAT1, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_D, 1);
  GETBAT (DBAT2, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_D, 2);
  GETBAT (DBAT3, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_D, 3);

  GETBAT (IBAT0, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_I, 0);
  GETBAT (IBAT1, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_I, 1);
  GETBAT (IBAT2, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_I, 2);
  GETBAT (IBAT3, bat.words.u, bat.words.l);
  bat_addrs_put (&bat, TYP_I, 3);


  if ( ppc_cpu_has_8_bats() && (HID0_7455_HIGH_BAT_EN & _read_HID0 ())) {
    GETBAT (DBAT4, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_D, 4);
    GETBAT (DBAT5, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_D, 5);
    GETBAT (DBAT6, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_D, 6);
    GETBAT (DBAT7, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_D, 7);
    GETBAT (IBAT4, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_I, 4);
    GETBAT (IBAT5, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_I, 5);
    GETBAT (IBAT6, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_I, 6);
    GETBAT (IBAT7, bat.words.u, bat.words.l);
    bat_addrs_put (&bat, TYP_I, 7);
  }
}

static void
do_dssall (void)
{
  /* Before changing BATs, 'dssall' must be issued.
   * We check MSR for MSR_VE and issue a 'dssall' if
   * MSR_VE is set hoping that
   *  a) on non-altivec CPUs MSR_VE reads as zero
   *  b) all altivec CPUs use the same bit
   * NOTE: psim doesn't implement dssall so we skip if we run on psim
   */
  if ( (_read_MSR () & MSR_VE) && PPC_PSIM != get_ppc_cpu_type() ) {
    /* this construct is needed because we don't know
     * if this file is compiled with -maltivec.
     * (I plan to add altivec support outside of
     * RTEMS core and hence I'd rather not
     * rely on consistent compiler flags).
     */
#define DSSALL	0x7e00066c      /* dssall opcode */
    __asm__ volatile ("	.long %0"::"i" (DSSALL));
#undef  DSSALL
  }
}

/* Clear I/D bats 4..7 ONLY ON 7455 etc.  */
static void
clear_hi_bats (void)
{
  do_dssall ();
  CLRBAT (DBAT4);
  CLRBAT (DBAT5);
  CLRBAT (DBAT6);
  CLRBAT (DBAT7);
  CLRBAT (IBAT4);
  CLRBAT (IBAT5);
  CLRBAT (IBAT6);
  CLRBAT (IBAT7);
}

static int
check_bat_index (int i)
{
  unsigned long hid0;

  if (i >= 0 && i < 4)
    return 0;
  if (i >= 4 && i < 8) {
    if ( ! ppc_cpu_has_8_bats() )
      return -1;
    /* OK, we're on the right hardware;
     * check if we are already enabled
     */
    hid0 = _read_HID0 ();
    if (HID0_7455_HIGH_BAT_EN & hid0)
      return 0;
    /* No; enable now */
    clear_hi_bats ();
    set_hid0_sync (hid0 | HID0_7455_HIGH_BAT_EN);
    return 0;
  }
  return -1;
}

/* size argument check:
 *  - must be a power of two or zero
 *  - must be <= 1<<28 ( non 745x cpu )
 *  - can be 1<<29..1<31 or 0xffffffff on 745x
 *  - size < 1<<17 means 0
 * computes and returns the block mask
 * RETURNS:
 *  block mask on success or -1 on error
 */
static int
check_bat_size (unsigned long size)
{
  unsigned long bit;
  unsigned long hid0;

  /* First of all, it must be a power of two */
  if (0 == size)
    return 0;

  if (0xffffffff == size) {
    bit = 32;
  } else {
    __asm__ volatile ("	cntlzw %0, %1":"=r" (bit):"r" (size));
    bit = 31 - bit;
    if (1 << bit != size)
      return -1;
  }
  /* bit < 17 is not really legal but we aliased it to 0 in the past */
  if (bit > (11 + 17)) {
    if ( ! ppc_cpu_has_8_bats() )
      return -1;

    hid0 = _read_HID0 ();
    /* Let's enable the larger block size if necessary */
    if (!(HID0_7455_XBSEN & hid0))
      set_hid0_sync (hid0 | HID0_7455_XBSEN);
  }

  return (1 << (bit - 17)) - 1;
}

static int
check_overlap (int typ, unsigned long start, unsigned long size)
{
  int i;
  unsigned long limit = start + size - 1;
  for (i = 0; i < sizeof (bat_addrs[typ]) / sizeof (bat_addrs[typ][0]); i++) {
    if (!((1 << i) & bat_in_use[typ]))
      continue;                 /* unused bat */
    /* safe is 'limit < bat_addrs[t][i].start || start > bat_addrs[t][i].limit */
    if (limit >= bat_addrs[typ][i].start && start <= bat_addrs[typ][i].limit)
      return i;
  }
  return -1;
}


/* Take no risks -- the essential parts of this routine run with
 * interrupts disabled!
 */

static int
setbat (int typ, int bat_index, unsigned long virt, unsigned long phys,
         unsigned int size, int flags)
{
  unsigned long level;
  unsigned int bl;
  int err;
  int wimgxpp;
  ubat bat;

  if (check_bat_index (bat_index)) {
    printk ("Invalid BAT index\n", bat_index);
    return -1;
  }

  if ((int) (bl = check_bat_size (size)) < 0) {
    printk ("Invalid BAT size\n", size);
    return -1;
  }

  if (virt & (size - 1)) {
    printk ("BAT effective address 0x%08x misaligned (size is 0x%08x)\n",
            virt, size);
    return -1;
  }

  if (phys & (size - 1)) {
    printk ("BAT physical address 0x%08x misaligned (size is 0x%08x)\n", phys,
            size);
    return -1;
  }

  if (virt + size - 1 < virt) {
    printk ("BAT range invalid: wraps around zero 0x%08x..0x%08x\n", virt,
            virt + size - 1);
    return -1;
  }

  if ( TYP_I == typ && ( ( _PAGE_GUARDED | _PAGE_WRITETHRU ) & flags ) ) {
  	printk("IBAT must not have 'guarded' or 'writethrough' attribute\n");
	return -1;
  }

/* must protect the bat_addrs table -- since this routine is only used for board setup
 * or similar special purposes we don't bother about interrupt latency too much.
 */
  rtems_interrupt_disable (level);

  {                             /* might have to initialize our cached data */
    static char init_done = 0;
    if (!init_done) {
      bat_addrs_init ();
      init_done = 1;
    }
  }

  err = check_overlap (typ, virt, size);
  if ((size >= (1 << 17)) && (err >= 0) && (err != bat_index)) {
    rtems_interrupt_enable (level);
    printk ("BATs must not overlap; area 0x%08x..0x%08x hits %cBAT %i\n",
            virt, virt + size, (TYP_I == typ ? 'I' : 'D'), err);
    return -1;
  }

  /* 603, 604, etc. */
  wimgxpp = flags & (_PAGE_WRITETHRU | _PAGE_NO_CACHE
                     | _PAGE_COHERENT | _PAGE_GUARDED);
  wimgxpp |= (flags & _PAGE_RW) ? BPP_RW : BPP_RX;
  bat.words.u = virt | (bl << 2) | 2;   /* Vs=1, Vp=0 */
  bat.words.l = phys | wimgxpp;
  if (flags & _PAGE_USER)
    bat.bat.batu.vp = 1;
  bat_addrs[typ][bat_index].start = virt;
  bat_addrs[typ][bat_index].limit = virt + ((bl + 1) << 17) - 1;
  bat_addrs[typ][bat_index].phys = phys;
  bat_in_use[typ] |= 1 << bat_index;
  if (size < (1 << 17)) {
    /* size of 0 tells us to switch it off */
    bat.bat.batu.vp = 0;
    bat.bat.batu.vs = 0;
    bat_in_use[typ] &= ~(1 << bat_index);
    /* mimic old behavior when bl was 0 (bs==0 is actually legal; it doesnt
     * indicate a size of zero. We now accept bl==0 and look at the size.
     */
    bat_addrs[typ][bat_index].limit = virt;
  }
  do_dssall ();
  if ( TYP_I == typ ) {
	  switch (bat_index) {
		  case 0: asm_setibat0 (bat.words.u, bat.words.l); break;
		  case 1: asm_setibat1 (bat.words.u, bat.words.l); break;
		  case 2: asm_setibat2 (bat.words.u, bat.words.l); break;
		  case 3: asm_setibat3 (bat.words.u, bat.words.l); break;
			  /* cpu check already done in check_index */
		  case 4: asm_setibat4 (bat.words.u, bat.words.l); break;
		  case 5: asm_setibat5 (bat.words.u, bat.words.l); break;
		  case 6: asm_setibat6 (bat.words.u, bat.words.l); break;
		  case 7: asm_setibat7 (bat.words.u, bat.words.l); break;
		  default:                     /* should never get here anyways */
			  break;
	  }
  } else {
	  switch (bat_index) {
		  case 0: asm_setdbat0 (bat.words.u, bat.words.l); break;
		  case 1: asm_setdbat1 (bat.words.u, bat.words.l); break;
		  case 2: asm_setdbat2 (bat.words.u, bat.words.l); break;
		  case 3: asm_setdbat3 (bat.words.u, bat.words.l); break;
			  /* cpu check already done in check_index */
		  case 4: asm_setdbat4 (bat.words.u, bat.words.l); break;
		  case 5: asm_setdbat5 (bat.words.u, bat.words.l); break;
		  case 6: asm_setdbat6 (bat.words.u, bat.words.l); break;
		  case 7: asm_setdbat7 (bat.words.u, bat.words.l); break;
		  default:                     /* should never get here anyways */
			  break;
	  }
  }
  rtems_interrupt_enable (level);

  return 0;
}

static int
getbat (int typ, int idx, unsigned long *pu, unsigned long *pl)
{
  unsigned long u, l;

  if (check_bat_index (idx)) {
    printk ("Invalid BAT #%i\n", idx);
    return -1;
  }
  if ( TYP_I == typ ) {
	  switch (idx) {
		  case 0: GETBAT (IBAT0, u, l); break;
		  case 1: GETBAT (IBAT1, u, l); break;
		  case 2: GETBAT (IBAT2, u, l); break;
		  case 3: GETBAT (IBAT3, u, l); break;
				  /* cpu check already done in check_index */
		  case 4: GETBAT (IBAT4, u, l); break;
		  case 5: GETBAT (IBAT5, u, l); break;
		  case 6: GETBAT (IBAT6, u, l); break;
		  case 7: GETBAT (IBAT7, u, l); break;
		  default:                     /* should never get here anyways */
				  return -1;
	  }
  } else {
	  switch (idx) {
		  case 0: GETBAT (DBAT0, u, l); break;
		  case 1: GETBAT (DBAT1, u, l); break;
		  case 2: GETBAT (DBAT2, u, l); break;
		  case 3: GETBAT (DBAT3, u, l); break;
				  /* cpu check already done in check_index */
		  case 4: GETBAT (DBAT4, u, l); break;
		  case 5: GETBAT (DBAT5, u, l); break;
		  case 6: GETBAT (DBAT6, u, l); break;
		  case 7: GETBAT (DBAT7, u, l); break;
		  default:                     /* should never get here anyways */
				  return -1;
	  }
  }
  if (pu) {
    *pu = u;
  }
  if (pl) {
    *pl = l;
  }

  if (!pu && !pl) {
    /* dump */
    ubat b;
    b.words.u = u;
    b.words.l = l;
    printk ("Raw %cBAT %i contents; UPPER: (0x%08x)", (TYP_I == typ ? 'I' : 'D'), idx, u);
    printk (" BEPI: 0x%08x", b.bat.batu.bepi);
    printk (" BL: 0x%08x", (u >> 2) & ((1 << 15) - 1));
    printk (" VS: 0b%i", b.bat.batu.vs);
    printk (" VP: 0b%i", b.bat.batu.vp);
    printk ("\n");
    printk ("                     LOWER: (0x%08x)", l);
    printk ("  RPN: 0x%08x", b.bat.batl.brpn);
    printk (" wimg:   0b%1i%1i%1i%1i", b.bat.batl.w, b.bat.batl.i,
            b.bat.batl.m, b.bat.batl.g);
    printk (" PP: 0x%1x", b.bat.batl.pp);
    printk ("\n");
    printk ("Covering EA Range: ");
    if (bat_in_use[typ] & (1 << idx))
      printk ("0x%08x .. 0x%08x\n", bat_addrs[typ][idx].start,
              bat_addrs[typ][idx].limit);
    else
      printk ("<none> (BAT off)\n");

  }
  return u;
}

int
setdbat (int bat_index, unsigned long virt, unsigned long phys,
         unsigned int size, int flags)
{
	return setbat(TYP_D, bat_index, virt, phys, size, flags);
}

int
setibat (int bat_index, unsigned long virt, unsigned long phys,
         unsigned int size, int flags)
{
	return setbat(TYP_I, bat_index, virt, phys, size, flags);
}

int
getdbat (int idx, unsigned long *pu, unsigned long *pl)
{
	return getbat (TYP_D, idx, pu, pl);
}

int
getibat (int idx, unsigned long *pu, unsigned long *pl)
{
	return getbat (TYP_I, idx, pu, pl);
}
