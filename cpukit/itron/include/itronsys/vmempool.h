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

#ifndef __ITRON_VARIABLE_MEMORYPOOL_h_
#define __ITRON_VARIABLE_MEMORYPOOL_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Variable Memory Pool (cre_mpl) Structure
 */

typedef struct t_cmpl {
  VP    exinf;    /* extended information */
  ATR   mplatr;   /* memorypool attributes */
  INT   mplsz;    /* memorypool size */
  /* additional information may be included depending on the implementation */
} T_CMPL;

/*
 *  mplatr
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */

/*
 *  mplid
 */

#define TMPL_OS   (-4)   /* memorypool used by OS */

/*
 *  Reference Variable Memory Pool (ref_mpl) Structure
 */

typedef struct t_rmpl {
  VP        exinf;    /* extended information */
  BOOL_ID   wtsk;     /* indicates whether or not there are waiting tasks */
  INT       frsz;     /* total size of free memory */
  INT       maxsz;    /* size of largest contiguous memory */
  /* additional information may be included depending on the implementation */
} T_RMPL;

/*
 *  Variable Memory Pool Functions
 */

/*
 *  cre_mpl - Create Variable-Size Memorypool
 */

ER cre_mpl(
  ID mplid,
  T_CMPL *pk_cmpl
);

/*
 *  del_mpl - Delete Variable-Size Memorypool
 */

ER del_mpl(
  ID mplid
);

/*
 *  get_blk - Get Variable-Size Memory Block
 */

ER get_blk(
  VP *p_blk,
  ID mplid,
  INT blksz
);

/*
 *  pget_blk - Poll and Get Variable-Size Memory Block
 */

ER pget_blk(
  VP *p_blk,
  ID mplid,
  INT blksz
);

/*
 *  tget_blk - Get Variable-Size Memory Block with Timeout
 */

ER tget_blk(
  VP *p_blk,
  ID mplid,
  INT blksz,
  TMO tmout
);

/*
 *  rel_blk - Release Variable-Size Memory Block
 */

ER rel_blk(
  ID mplid,
  VP blk
);

/*
 *  ref_mpl - Reference Variable-Size Memorypool Status
 */

ER ref_mpl(
  T_RMPL *pk_rmpl,
  ID mplid
);


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

