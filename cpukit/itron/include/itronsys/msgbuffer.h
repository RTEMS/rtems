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

#ifndef __ITRON_MESSAGEBUFFER_h_
#define __ITRON_MESSAGEBUFFER_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create Message Buffer (cre_mbf) Structure
 */

typedef struct t_cmbf {
  VP    exinf;    /* extended information */
  ATR   mbfatr;   /* messagebuffer attributes */
  INT   bufsz;    /* messagebuffer size */
  INT   maxmsz;   /* maximum size of messages */
  /* additional information may be included depending on the implementation */
} T_CMBF;

/*
 *  mbfatr
 */

#define TA_TFIFO   0x00   /* tasks waiting to receive messages are handled
                                 by FIFO */
#define TA_TPRI    0x01   /* tasks waiting to receive messages are handled
                                 by priority */

/*
 *  mbfid
 */

#define TMBF_OS   (-4)   /* messagebuffer used for OS error log */
#define TMBF_DB   (-3)   /* messagebuffer used for debugging */

/*
 *  Reference Message Buffer (ref_mbf) Structure
 */

typedef struct t_rmbf {
  VP        exinf;     /* extended information */
  BOOL_ID   wtsk;      /* indicates whether or not there is a */
                       /* task waiting to receive a message */
  BOOL_ID   stsk;      /* indicates whether or not there is a */
                       /* task waiting to send a message */
  INT       msgsz;     /* size of message to be sent next */
  INT       frbufsz;   /* size of free buffer */
  /* additional information may be included depending on the implementation */
} T_RMBF;

/*
 *  Message Buffer Functions
 */

/*
 *  cre_mbf - Create MessageBuffer
 */

ER cre_mbf(
  ID mbfid,
  T_CMBF *pk_cmbf
);

/*
 *  del_mbf - Delete MessageBuffer
 */

ER del_mbf(
  ID mbfid
);

/*
 *  snd_mbf - Send Message to MessageBuffer
 */

ER snd_mbf(
  ID mbfid,
  VP msg,
  INT msgsz
);

/*
 *  psnd_mbf - Poll and Send Message to MessageBuffer
 */

ER psnd_mbf(
  ID mbfid,
  VP msg,
  INT msgsz
);

/*
 *  tsnd_mbf - Send Message to MessageBuffer with Timeout
 */

ER tsnd_mbf(
  ID mbfid,
  VP msg,
  INT msgsz,
  TMO tmout
);

/*
 *  rcv_mbf - Receive Message from MessageBuffer
 */

ER rcv_mbf(
  VP msg,
  INT *p_msgsz,
  ID mbfid
);

/*
 *  prcv_mbf - Poll and Receive Message from MessageBuffer
 */

ER prcv_mbf(
  VP msg,
  INT *p_msgsz,
  ID mbfid
);

/*
 *  trcv_mbf - Receive Message from MessageBuffer with Timeout
 */

ER trcv_mbf(
  VP msg,
  INT *p_msgsz,
  ID mbfid,
  TMO tmout
);

/*
 *  ref_mbf - Reference MessageBuffer Status
 */

ER ref_mbf(
  T_RMBF *pk_rmbf,
  ID mbfid
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

