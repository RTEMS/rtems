/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  This is where the real hardware setup is done. A minimal stack
 *  has been provided by the start.S code. No normal C or RTEMS
 *  functions can be called from here.
 */

#include <stdint.h>

extern void _wr_vbr(uint32_t);
extern int boot_card(int, char **, char **);

extern long _d0_reset,_d1_reset,_M68kSpuriousInterruptCount;

/*
 * From linkcmds
 */

extern uint8_t _VBR[];
extern uint8_t _INTERRUPT_VECTOR[];

extern uint8_t _clear_start[];
extern uint8_t _clear_end[];

extern uint8_t _data_src_start[];
extern uint8_t _data_dest_start[];
extern uint8_t _data_dest_end[];

void Init5225x(void)
{
  register uint32_t i;
  register uint32_t *dp, *sp;
  register uint8_t *dbp, *sbp;

  /* 
   * Copy the vector table to RAM 
   */

  if (_VBR != _INTERRUPT_VECTOR) {
    sp = (uint32_t *) _INTERRUPT_VECTOR;
    dp = (uint32_t *) _VBR;
    for (i = 0; i < 256; i++) {
      *dp++ = *sp++;
    }
  }

  /* 
   * Move initialized data from ROM to RAM. 
   */
  if (_data_src_start != _data_dest_start) {
    dbp = (uint8_t *) _data_dest_start;
    sbp = (uint8_t *) _data_src_start;
    i = _data_dest_end - _data_dest_start;
    while (i--)
      *dbp++ = *sbp++;
  }

	asm __volatile__ ("move.l %%d5,%0\n\t":"=r" (_d0_reset));
	asm __volatile__ ("move.l %%d6,%0\n\t":"=r" (_d1_reset));
  
  /* 
   * Zero uninitialized data 
   */

  if (_clear_start != _clear_end) {
    sbp = _clear_start;
    dbp = _clear_end;
    i = dbp - sbp;
    while (i--)
      *sbp++ = 0;
  }

//_wr_vbr((uint32_t) _VBR);
	asm volatile("move.l %0,%%d7;movec %%d7,%%vbr\n\t"::"i"(_VBR): "cc");

  /*
   * We have to call some kind of RTEMS function here!
   */

  boot_card(0, 0, 0);
  for (;;) ;
}
