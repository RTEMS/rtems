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

#ifndef __ITRON_SEMAPHORE_h_
#define __ITRON_SEMAPHORE_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Semaphore (cre_sem) Structure
 */

typedef struct t_csem {
  VP    exinf;    /* extended information */
  ATR   sematr;   /* semaphore attributes */
  /* Following is the extended function for [level X]. */
  INT   isemcnt;   /* initial semaphore count */
  INT   maxsem;    /* maximum semaphore count */
  /* additional information may be included depending on the implementation */
} T_CSEM;

/*
 *  sematr - Semaphore Attribute Values
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */

#define _ITRON_SEMAPHORE_UNUSED_ATTRIBUTES ~(TA_TPRI)

/*
 *  Reference Semaphore (ref_sem) Structure
 */

typedef struct t_rsem {
  VP      exinf;    /* extended information */
  BOOL_ID wtsk;     /* indicates whether or not there is a waiting task */
  INT     semcnt;   /* current semaphore count */
  /* additional information may be included depending on the implementation */
} T_RSEM;

/*
 *  Semaphore Functions
 */

/*
 *  cre_sem - Create Semaphore
 */

ER cre_sem(
  ID semid,
  T_CSEM *pk_csem
);

/*
 *  del_sem - Delete Semaphore
 */

ER del_sem(
  ID semid
);

/*
 *  sig_sem - Signal Semaphore
 */

ER sig_sem(
  ID semid
);

/*
 *  wai_sem - Wait on Semaphore 
 */

ER wai_sem(
  ID semid
);

/*
 *  preq_sem - Poll and Request Semaphore
 */

ER preq_sem(
  ID semid
);

/*
 *  twai_sem - Wait on Semaphore with Timeout
 */

ER twai_sem(
  ID semid,
  TMO tmout
);

/*
 *  ref_sem - Reference Semaphore Status
 */

ER ref_sem(
  ID      semid,
  T_RSEM *pk_rsem
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

