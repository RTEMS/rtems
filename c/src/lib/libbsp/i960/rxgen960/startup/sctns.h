/*-------------------------------------*/
/* sctns.h                             */
/* Last change : 10.10.94              */
/*-------------------------------------*/
/*
 *  $Id$
 */

#ifndef _SCTNS_H_
#define _SCTNS_H_

  /* Copy all code into SRAM.
   * Fault Table and Fault Handler stays in EPROM to not be 
   * destroyed by a buggy user program. Beyond that only
   * monitor Start point and procedures to copy code
   * into RAM will be relocated in ROM.
   */
extern void copyCodeToRam(void);
  /* Zero uninitialized section.
   */
extern void zeroBss(void);

  /* Some relocation symbols. These
   * symbols are defined in rom.ld.
   */
extern unsigned int codeRomStart[];
extern unsigned int codeRamStart[];
extern unsigned int codeRamEnd[];

extern unsigned int bssStart[];
extern unsigned int bssEnd[]; 

#endif
/*-------------*/
/* End of file */
/*-------------*/

