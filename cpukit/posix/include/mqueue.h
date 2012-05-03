/**
 * @file mqueue.h
 *
 * This file contains the definitions related to POSIX Message Queues.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _MQUEUE_H
#define _MQUEUE_H


#include <unistd.h>

#if defined(_POSIX_MESSAGE_PASSING)

#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  15.1.1 Data Structures, P1003.1b-1993, p. 271
 */

/**
 *  Message queue id type.
 *
 *  @note Use uint32_t since all POSIX Ids are 32-bit currently.
 */
typedef uint32_t  mqd_t;

/**
 *  This is the message queue attributes structure.
 */
struct mq_attr {
  /** This is the message queue flags */
  long  mq_flags;
  /** This is the maximum number of messages */
  long  mq_maxmsg;
  /** This is the maximum message size */
  long  mq_msgsize;
  /** This is the mumber of messages currently queued */
  long  mq_curmsgs;
};

/**
 *  15.2.2 Open a Message Queue, P1003.1b-1993, p. 272
 */
mqd_t mq_open(
  const char *name,
  int         oflag,
  ...
);

/**
 *  15.2.2 Close a Message Queue, P1003.1b-1993, p. 275
 */
int mq_close(
  mqd_t  mqdes
);

/**
 *  15.2.2 Remove a Message Queue, P1003.1b-1993, p. 276
 */
int mq_unlink(
  const char *name
);

/**
 *  15.2.4 Send a Message to a Message Queue, P1003.1b-1993, p. 277
 *
 *  @note P1003.4b/D8, p. 45 adds mq_timedsend().
 */
int mq_send(
  mqd_t         mqdes,
  const char   *msg_ptr,
  size_t        msg_len,
  unsigned int  msg_prio
);

#if defined(_POSIX_TIMEOUTS)

#include <time.h>

int mq_timedsend(
  mqd_t                  mqdes,
  const char            *msg_ptr,
  size_t                 msg_len,
  unsigned int           msg_prio,
  const struct timespec *abstime
);

#endif /* _POSIX_TIMEOUTS */

/*
 *  15.2.5 Receive a Message From a Message Queue, P1003.1b-1993, p. 279
 *
 *  NOTE: P1003.4b/D8, p. 45 adds mq_timedreceive().
 */

ssize_t mq_receive(
  mqd_t         mqdes,
  char         *msg_ptr,
  size_t        msg_len,
  unsigned int *msg_prio
);

#if defined(_POSIX_TIMEOUTS)

ssize_t mq_timedreceive(
  mqd_t                  mqdes,
  char                  *msg_ptr,
  size_t                 msg_len,
  unsigned int          *msg_prio,
  const struct timespec *abstime
);

#endif /* _POSIX_TIMEOUTS */

#if defined(_POSIX_REALTIME_SIGNALS)

/*
 *  15.2.6 Notify Process that a Message is Available on a Queue,
 *         P1003.1b-1993, p. 280
 */

int mq_notify(
  mqd_t                  mqdes,
  const struct sigevent *notification
);

#endif /* _POSIX_REALTIME_SIGNALS */

/*
 *  15.2.7 Set Message Queue Attributes, P1003.1b-1993, p. 281
 */

int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *mqstat,
  struct mq_attr       *omqstat
);

/*
 *  15.2.8 Get Message Queue Attributes, P1003.1b-1993, p. 283
 */

int mq_getattr(
  mqd_t           mqdes,
  struct mq_attr *mqstat
);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_MESSAGE_PASSING */

#endif
/* end of include file */
