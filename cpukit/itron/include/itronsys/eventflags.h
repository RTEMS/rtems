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

#ifndef __ITRON_EVENTFLAGS_h_
#define __ITRON_EVENTFLAGS_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Eventflags (cre_flg) Structure
 */

typedef struct t_cflg {
  VP     exinf;     /* extended information */
  ATR    flgatr;    /* eventflag attribute */
  UINT   iflgptn;   /* initial eventflag */
  /* additional information may be included depending on the implementation */
} T_CFLG;

/*
 *  flgatr
 */

#define TA_WSGL   0x00   /* multiple tasks are not allowed to wait (Wait
                                Single Task) */
#define TA_WMUL   0x08   /* multiple tasks are allowed to wait (Wait
                                Multiple Task) */

/*
 *  wfmode
 */
 
#define TWF_ANDW   0x00   /* AND wait */
#define TWF_ORW    0x02   /* OR wait */
#define TWF_CLR    0x01   /* clear specification */

/*
 *  Reference Eventflags (ref_flg) Structure
 */

typedef struct t_rflg {
  VP        exinf;      /* extended information */
  BOOL_ID   wtsk;       /* indicates whether or not there is a waiting task */
  UINT      flgptn;     /* eventflag bit pattern */
  /* additional information may be included depending on the implementation */
} T_RFLG;

/*
 *  Eventflag Functions
 */

/*
 *  cre_flg - Create Eventflag
 */

ER cre_flg(
  ID flgid,
  T_CFLG *pk_cflg
);

/*
 *  del_flg - Delete Eventflag
 */

ER del_flg(
  ID flgid
);

/*
 *  set_flg - Set Eventflag
 */

ER set_flg(
  ID flgid,
  UINT setptn
);

/*
 *  clr_flg - Clear Eventflag
 */

ER clr_flg(
  ID flgid,
  UINT clrptn
);

/*
 *  wai_flg - Wait on Eventflag
 */

ER wai_flg(
  UINT *p_flgptn,
  ID flgid,
  UINT waiptn,
  UINT wfmode
);

/*
 *  pol_flg - Wait for Eventflag(Polling)
 */

ER pol_flg(
  UINT *p_flgptn,
  ID flgid,
  UINT waiptn,
  UINT wfmode
);

/*
 *  twai_flg - Wait on Eventflag with Timeout
 */

ER twai_flg(
  UINT *p_flgptn,
  ID flgid,
  UINT waiptn,
  UINT wfmode,
  TMO tmout
);

/*
 *  ref_flg - Reference Eventflag Status
 */

ER ref_flg(
  T_RFLG *pk_rflg,
  ID flgid
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

