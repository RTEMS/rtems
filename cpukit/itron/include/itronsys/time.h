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

#ifndef __ITRON_TIME_h_
#define __ITRON_TIME_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  System Time Data Type
 *
 *  Member configuration depends on the bit width of the processor and
 *  on the implementation.  A total of 48 bits is recommended.
 *
 */

typedef struct t_systime {
  H    utime;   /* upper 16 bits */
  UW   ltime;   /* lower 32 bits */
} SYSTIME, CYCTIME, ALMTIME;

/*
 *  XXX Look this up in the spec and figure out where it comes
 *  XXX from.  dly_tim() references it but it was left out
 *  XXX of the initial cut at the header files.
 */

typedef int DLYTIME;

/*
 *  XXX (def_cyc) Structure
 */

typedef struct t_dcyc {
  VP        exinf;    /* extended information */
  ATR       cycatr;   /* cyclic handler attributes */
  FP        cychdr;   /* cyclic handler address */
  UINT      cycact;   /* cyclic handler activation */
  CYCTIME   cyctim;   /* cyclic startup period */
} T_DCYC;

/*
 *  cycact
 */

#define TCY_OFF   0x00   /* do not invoke cyclic handler */
#define TCY_ON    0x01   /* invoke cyclic handler */
#define TCY_INT   0x02   /* initialize cycle count */

/*
 *  Reference Cyclic Handler (ref_cyc) Structure
 */

typedef struct t_rcyc {
  VP        exinf;    /* extended information */
  CYCTIME   lfttim;   /* time left before next handler startup */
  UINT      cycact;   /* cyclic handler activation */
  /* additional information may be included depending on the implementation */
} T_RCYC;

/*
 *  Define Alarm (def_alm) Structure
 */

typedef struct t_dalm {
  VP        exinf;    /* extended information */
  ATR       almatr;   /* alarm handler attributes */
  FP        almhdr;   /* alarm handler address */
  UINT      tmmode;   /* start time specification mode */
  ALMTIME   almtim;   /* handler startup time */
} T_DALM;

/*
 *  tmmode
 */

#define TTM_ABS   0x00   /* specified as an absolute time */
#define TTM_REL   0x01   /* specified as a relative time */

/*
 *  Reference Alarm (ref_alm) Structure
 */

typedef struct t_ralm {
  VP        exinf;    /* extended information */
  ALMTIME   lfttim;   /* time left before next handler startup */
  /* additional information may be included depending on the implementation */
} T_RALM;

/*
 *  Time Management Functions
 */

/*
 *  set_tim - Set System Clock
 */

ER set_tim(
  SYSTIME *pk_tim
);

/*
 *  get_tim - Get System Clock
 */

ER get_tim(
  SYSTIME *pk_tim
);

/*
 *  dly_tsk - Delay Task
 */

ER dly_tsk(
  DLYTIME dlytim
);

/*
 *  def_cyc - Define Cyclic Handler
 */

ER def_cyc(
  HNO cycno,
  T_DCYC *pk_dcyc
);

/*
 *  act_cyc - Activate Cyclic Handler
 */

ER act_cyc(
  HNO cycno,
  UINT cycact
);

/*
 *  ref_cyc - Reference Cyclic Handler Status
 */

ER ref_cyc(
  T_RCYC *pk_rcyc,
  HNO cycno
);

/*
 *  def_alm - Define Alarm Handler
 */

ER def_alm(
  HNO almno,
  T_DALM *pk_dalm
);

/*
 *  ref_alm - Reference Alarm Handler Status
 */

ER ref_alm(
  T_RALM *pk_ralm,
  HNO almno
);

/*
 *  ret_tmr - Return from Timer Handler
 */

void ret_tmr( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

