/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __ITRON_SYSTEM_MANAGEMENT_h_
#define __ITRON_SYSTEM_MANAGEMENT_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Get Version (get_ver) Structure
 */

typedef struct t_ver {
  UH   maker;     /* vendor */
  UH   id;        /* format number */
  UH   spver;     /* specification version */
  UH   prver;     /* product version */
  UH   prno[4];   /* product control information */
  UH   cpu;       /* CPU information */
  UH   var;       /* variation descriptor */
} T_VER;

/*
 *  Specific MAKER codes established as of March, 1993 are as follows.
 *  Due to restrictions on the assignment of CPU codes described below, it is
 *  necessary to use maker codes in the range 0x000 through 0x00ff for vendors
 *  developing CPUs.
 */

/*
 *  CPU defines XXX need to name the constants
 */

#if 0
#define 0x000  /* No version (test systems, etc.) */
#define 0x001  /* University of Tokyo */
#define 0x009  /* FUJITSU LIMITED */
#define 0x00a  /* Hitachi, Ltd. */
#define 0x00b  /* Matsushita Electric Industrial Co., Ltd. */
#define 0x00c  /* Mitsubishi Electric Corporation */
#define 0x00d  /* NEC Corporation */
#define 0x00e  /* Oki Electric Industry Co., Ltd. */
#define 0x00f  /* TOSHIBA CORPORATION */
#endif

/*
 * The above have been assigned in alphabetical order.
 */

#if 0
#define 0x010  /* ALPS ELECTRIC CO., LTD. */
#define 0x011  /* WACOM Co., Ltd. */
#define 0x012  /* Personal Media Corporation */
#define 0x101  /* OMRON CORPORATION */
#define 0x102  /* SEIKOSHA CO., LTD. */
#define 0x103  /* SYSTEM ALGO CO., LTD. */
#define 0x104  /* Tokyo Computer Service Co., Ltd. */
#define 0x105  /* YAMAHA CORPORATION */
#define 0x106  /* MORSON JAPAN */
#define 0x107  /* TOSHIBA INFORMATION SYSTEMS (JAPAN) CORP. */
#define 0x108  /* Miyazaki System Planning Office */
#define 0x109  /* Three Ace Computer Corporation */
#endif

/*
 *  CPU Codes
 *
 *  Figure 47 shows the format of cpu code.  Some processors use the format
 *  given in Figure 47(1).  The format given in Figure 47(2) is used for all
 *  other proprietary processors.
 *
 *  The code assignment of the CPU1 region in the format given in Figure 47(1)
 *  is common to ITRON and BTRON specifications.  The same number is used in
 *  the CPU type of the standard object format of BTRON specification
 *  operating systems implemented on a TRON-specification chip.
 *
 *  When using the format given in Figure 47(2) the code used for MAKER1 is
 *  assigned by using the lower 8 bits of MAKER described in the previous
 *  subsection.  The code assignment of CPU2 is left up to each maker.
 *  
 *  
 *  
 *        +---------------+---------------+---------------+---------------+
 *    (1) | 0   0   0   0   0   0   0   0 |              CPU1             |
 *        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *        +---------------+---------------+---------------+---------------+
 *    (2) |             MAKER1            |              CPU2             |
 *        +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *  
 *                   Figure 47 Format of cpu Returned by get_ver
 *  
 *  
 *  Specific CPU codes established as of March, 1993 are as follows.
 */

#if 0
/*
 * XXX CONVERT THESE to #defines
 */


/*
 *  Contents of the CPU1 field
 */

#define (0x0)   CPU unspecified, no CPU information given
#define (0x1)   TRONCHIP32 shared
#define (0x2)   reserved
#define (0x3)   reserved
#define (0x4)   reserved
#define (0x5)   reserved (<<L1R>> TRON-specification chip)
#define (0x6)   reserved (<<L1>> TRON-specification chip)
#define (0x7)   reserved (TRON-specification chip supporting the
                             LSID function)
/* CPU vendors are unspecified for codes B'00000000 through B'00000111. */

#define (0x8)  reserved
#define (0x9)  GMICRO/100
#define (0xa)  GMICRO/200
#define (0xb)  GMICRO/300
#define (0xc)  reserved
#define (0xd)  TX1
#define (0xe)  TX2
#define (0xf)  reserved

#define (0x10)  reserved
#define (0x11)  reserved
#define (0x12)  reserved
#define (0x13)  O32
#define (0x14)  reserved
#define (0x15)  MN10400
#define (0x16)  reserved
#define (0x17)  reserved

#define (0x18)  GMICRO/400
#define (0x19)  GMICRO/500
#define (0x1a)  reserved
#define (0x1b-0x3f)
                            reserved
          * For GMICRO extended, TX series extended, and TRONCHIP64 chips.

#define (0x40)   Motorola 68000
#define (0x41)   Motorola 68010
#define (0x42)   Motorola 68020
#define (0x43)   Motorola 68030
#define (0x44)   Motorola 68040
#define -(0x40-0x4f)
#define                       Motorola 68000 family
#define (0x50)   National Semiconductor NS32032
#define (0x50-0x5f)
                              National Semiconductor NS32000 family
#define (0x60)   Intel 8086, 8088
#define (0x61)   Intel 80186
#define (0x62)   Intel 80286
#define (0x63)   Intel 80386
#define (0x64)   Intel 80486
#define (0x60-0x6f)
                              Intel iAPX86 family

#define (0x70-0x7f)
                              NEC V Series

#define (0x80-0xff)
                              reserved
#endif

/*
 *  Assigning Version Numbers
 *
 *  The version numbers of ITRON and uITRON specifications take the following
 *  form.
 *  
 *          Ver X.YY.ZZ[.WW]
 *  
 *  where "X" represents major version number of the ITRON specification to
 *  distinguish ITRON1, ITRON2 and uITRON 3.0 specifications.  Specific
 *  assignment is as follows.
 *  
 *          "X" = 1  ITRON1 specification
 *              = 2  ITRON2 or uITRON 2.0 specification
 *              = 3  uITRON 3.0 specification
 *  
 *  "YY" is a number used to distinguish versions according to changes and
 *  additions made to the specification.  After the specification is published,
 *  this number is incremented in order "YY" = 00, 01, 02... according to
 *  version upgrades.  The first digit of "YY" is 'A', 'B' or 'C' for draft
 *  standard versions and test versions within the TRON Association before the
 *  specification have been published.
 *  
 *  The "X.YY" part of the specification version numbers is returned by spver
 *  to get_ver system call.  The corresponding hexadecimal value is used when
 *  "YY" includes 'A', 'B' or 'C'.
 *  
 *  "ZZ" represents a number used to distinguish versions related to the written
 *  style of a specification.  This number is incremented in order
 *  "ZZ" = 00, 01, 02... when there have been changes in specification
 *  configuration, reordering of chapters or corrections of misprints.  
 *  When a further distinction of the written style of specifications is
 *  desired, ".WW" may be added optionally after "ZZ".  WW will be assumed
 *  to be zero if ".WW" is omitted.
 */

/*
 *  Reference System (ref_sys) Structure
 */

typedef struct t_rsys {
  INT   sysstat;   /* system state */
  /* additional information may be included depending on the implementation */
} T_RSYS;

/*
 *  sysstat
 */

#define TSS_TSK    0   /* normal state in which dispatching is enabled during
                          task portion execution */
#define TSS_DDSP   1   /* state after dis_dsp has been executed during task
                          portion execution (dispatch disabled) */
#define TSS_LOC    3   /* state after loc_cpu has been executed during task
                          portion execution (interrupt and dispatch disabled)
                          */
#define TSS_INDP   4   /* state during execution of task-independent portions
                          (interrupt and timer handlers) */

/*
 *  Reference Configuration (ref_cfg) Structure
 */

typedef struct t_rcfg {
  /* details concerning members are implementation dependent */
} T_RCFG;

/*
 *  Define Service (def_svc) Structure
 */

typedef struct t_dsvc {
  ATR   svcatr;   /* extended SVC handler attributes */
  FP    svchdr;   /* extended SVC handler address */
  /* additional information may be included depending on the implementation */
} T_DSVC;

/*
 *  Define Exception (def_exc) Structure
 */

typedef struct t_dexc {
  ATR   excatr;   /* exception handler attributes */
  FP    exchdr;   /* exception handler address */
  /* additional information may be included depending on the implementation */
} T_DEXC;

/*
 *  System Management Functions
 */

/*
 *  get_ver - Get Version Information
 */

ER get_ver(
  T_VER *pk_ver
);

/*
 *  ref_sys - Reference Semaphore Status
 */

ER ref_sys(
  T_RSYS *pk_rsys
);

/*
 *  ref_cfg - Reference Configuration Information
 */

ER ref_cfg(
  T_RCFG *pk_rcfg
);

/*
 *  def_svc - Define Extended SVC Handler
 */

ER def_svc(
  FN s_fncd,
  T_DSVC *pk_dsvc
);

/*
 *  def_exc - Define Exception Handler
 */

ER def_exc(
  UINT exckind,
  T_DEXC *pk_dexc
);




#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

