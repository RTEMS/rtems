/*
 * bat.c
 *
 *	    This file contains the implementation of C function to
 *          Instanciate 60x/7xx ppc Block Address Translation (BAT) registers.
 *	    More detailled information can be found on motorola
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
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */

#include <libcpu/bat.h>

typedef union {			/* BAT register values to be loaded */
	BAT		bat;
	unsigned int	word[2];
}ubat;

typedef struct batrange {		/* stores address ranges mapped by BATs */
	unsigned long start;
	unsigned long limit;
	unsigned long phys;
}batrange;

batrange bat_addrs[4];

void setdbat(int bat_index, unsigned long virt, unsigned long phys,
       unsigned int size, int flags)
{
  unsigned int bl;
  int wimgxpp;
  ubat bat;

  bl = (size >> 17) - 1;
  /* 603, 604, etc. */
  wimgxpp = flags & (_PAGE_WRITETHRU | _PAGE_NO_CACHE
		     | _PAGE_COHERENT | _PAGE_GUARDED);
  wimgxpp |= (flags & _PAGE_RW)? BPP_RW: BPP_RX;
  bat.word[0] = virt | (bl << 2) | 2; /* Vs=1, Vp=0 */
  bat.word[1] = phys | wimgxpp;
  if (flags & _PAGE_USER)
    bat.bat.batu.vp = 1;
  bat_addrs[bat_index].start = virt;
  bat_addrs[bat_index].limit = virt + ((bl + 1) << 17) - 1;
  bat_addrs[bat_index].phys = phys;
  switch (bat_index) {
  case 0 : asm_setdbat0(bat.word[0], bat.word[1]); break;
  case 1 : asm_setdbat1(bat.word[0], bat.word[1]); break;
  case 2 : asm_setdbat2(bat.word[0], bat.word[1]); break;
  case 3 : asm_setdbat3(bat.word[0], bat.word[1]); break;
  default: printk("bat.c : invalid BAT bat_index\n");
  }
}

