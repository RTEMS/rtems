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

#ifndef __ITRON_MBOX_h_
#define __ITRON_MBOX_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Mailbox (cre_mbx) Structure
 */

typedef struct t_cmbx {
  VP    exinf;    /* extended information */
  ATR   mbxatr;   /* mailbox attributes */
  /* Following is implementation-dependent function */
  INT   bufcnt;   /* maximum number of messages to let pend */
  /* additional information may be included depending on the implementation */
} T_CMBX;

/*
 *  mbxatr
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */
#define TA_MFIFO   0x00   /* messages are handled by FIFO */
#define TA_MPRI    0x02   /* messages are handled by priority */

typedef struct t_msg {
  /*
   *  A region (header) reserved by the OS may be included first
   *  depending on the implementation.
   *
   *  NOTE: The first two fields are RTEMS specific.
   */

  INT       msgpri;   /* priority of each message */
  /* VB   msgcont[];  XXX */
  unsigned8 msgcont[1];
} T_MSG;

/*
 *  Reference Mailbox (ref_mbx) Structure
 */

typedef struct t_rmbx {
  VP        exinf;    /* extended information */
  BOOL_ID   wtsk;     /* indicates whether or not there is a waiting task */
  T_MSG    *pk_msg;   /* message to be sent next */
  /* additional information may be included depending on the implementation */
} T_RMBX;

/*
 *  Mailbox Functions
 */

/*
 *  cre_mbx - Create Mailbox
 */

ER cre_mbx(
  ID      mbxid,
  T_CMBX *pk_cmbx
);

/*
 *  del_mbx - Delete Mailbox
 */

ER del_mbx(
  ID mbxid
);

/*
 *  snd_msg - Send Message to Mailbox
 */

ER snd_msg(
  ID     mbxid,
  T_MSG *pk_msg
);

/*
 *  rcv_msg - Receive Message from Mailbox 
 */

ER rcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
);

/*
 *  prcv_msg - Poll and Receive Message from Mailbox
 */

ER prcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
);

/*
 *  trcv_msg - Receive Message from Mailbox with Timeout
 */

ER trcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid,
  TMO     tmout
);

/*
 *  ref_mbx - Reference Mailbox Status
 */

ER ref_mbx(
  T_RMBX *pk_rmbx,
  ID      mbxid
);


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

