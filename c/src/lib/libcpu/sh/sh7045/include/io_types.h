/************************************************************************
 *
 *  Data types and constants for Hitachi SH704X on-chip peripherals
 *
 *  Author: John M.Mills (jmills@tga.com)
 *
 *  COPYRIGHT (c) 1999, TGA Technologies, Norcross, GA, USA
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  This file may be distributed as part of the RTEMS software item.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *      John M. Mills (jmills@tga.com)
 *      TGA Technologies, Inc.
 *      100 Pinnacle Way, Suite 140
 *      Norcross, GA 30071 U.S.A.
 *
 *      This modified file may be copied and distributed in accordance
 *      the above-referenced license. It is provided for critique and
 *      developmental purposes without any warranty nor representation
 *      by the authors or by TGA Technologies.
 *
 *
 ************************************************************************/

#ifndef _sh_io_types_h
#define _sh_io_types_h

#include <rtems/score/iosh7045.h>
#include <termios.h>

typedef enum {SCI0, SCI1} portNo;
typedef enum {eight, seven} dataBits;
typedef enum {one, two} stopBits;
typedef enum {even, odd} parity;

typedef struct {
  portNo   line;
  int      speed_ix;
  dataBits dBits;
  int      parEn;
  parity   par;
  int      mulPro;
  stopBits sBits;
} sci_setup_t;

typedef union{
	unsigned char Reg;         /* By Register */
	struct {                   /* By  Field */
		 unsigned char Sync  :1; /* Async/Sync */
		 unsigned char DBts  :1; /* Char.Length */
		 unsigned char ParEn :1; /* Parity En.*/
		 unsigned char Odd   :1; /* Even/Odd */
		 unsigned char SBts  :1; /* No.Stop Bits */
		 unsigned char MulP  :1; /* Multi-Proc. */
		 unsigned char Dvsr  :2; /* Clock Sel. */
	} Fld;
} sci_smr_t;

typedef union {
	unsigned char Reg;          /*  By Register */
	struct {                    /*  By Field */
		 unsigned char TIE :1;    /*  Tx.Int.En. */
		 unsigned char RIE :1;    /*  Rx.Int.En. */
		 unsigned char TE  :1;    /*  Tx.En. */
		 unsigned char RE  :1;    /*  Rx.En. */
		 unsigned char MPIE:1;    /*  Mult.Pro.Int.En. */
		 unsigned char TEIE:1;    /*  Tx.End Int.En. */
		 unsigned char CkSrc :2;   /*  Clock Src. */
	} Fld;
} sci_scr_t;

typedef struct {
  unsigned char n ;
  unsigned char N ;
} sci_bitrate_t;

#endif /* _sh_io_types_h */
