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

#ifndef __ITRON_FIXED_MEMORYPOOL_h_
#define __ITRON_FIXED_MEMORYPOOL_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Fixed Memory Pool (cre_mpf) Structure
 */

    /* cre_mpf */
typedef struct t_cmpf {
  VP    exinf;     /* extended information */
  ATR   mpfatr;    /* memorypool attributes */
  INT   mpfcnt;    /* block count for entire memorypool */
  INT   blfsz;     /* fixed-size memory block size */
  /* additional information may be included depending on the implementation */
} T_CMPF;

/*
 *  mpfatr
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */


/*
 *  Reference Fixed Memory Pool (ref_mpf) Structure
 */

    /* ref_mpf */
typedef struct t_rmpf {
  VP        exinf;    /* extended information */
  BOOL_ID   wtsk;     /* indicates whether or not there are waiting tasks */
  INT       frbcnt;   /* free block count */
  /* additional information may be included depending on the implementation */
} T_RMPF;

/*
 *  Fixed Memory Pool Functions
 */

/*
 *  cre_mpf - Create Fixed-Size Memorypool
 */

ER cre_mpf(
  ID mpfid,
  T_CMPF *pk_cmpf
);

/*
 *  del_mpf - Delete Fixed-Size Memorypool
 */

ER del_mpf(
  ID mpfid
);

/*
 *  get_blf - Get Fixed-Size Memory Block
 */

ER get_blf(
  VP *p_blf,
  ID mpfid
);

/*
 *  pget_blf - Poll and Get Fixed-Size Memory Block
 */

ER pget_blf(
  VP *p_blf,
  ID mpfid
);

/*
 *  tget_blf - Get Fixed-Size Memory Block with Timeout
 */

ER tget_blf(
  VP *p_blf,
  ID mpfid,
  TMO tmout
);

/*
 *  rel_blf - Release Fixed-Size Memory Block
 */

ER rel_blf(
  ID mpfid,
  VP blf
);

/*
 *  ref_mpf - Reference Fixed-Size Memorypool Status
 */

ER ref_mpf(
  T_RMPF *pk_rmpf,
  ID mpfid
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

